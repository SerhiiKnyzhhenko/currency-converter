import { CURRENCIES, TRANSLATIONS } from './currencies-generated.js';


// Менеджер состояния приложения
class AppState {
    constructor() {
        this._lang = 'ua';
        this._subscribers = [];
    }

    get lang() {
        return this._lang;
    }

    set lang(newLang) {
        const oldLang = this._lang;
        this._lang = newLang;
        this._subscribers.forEach(cb => cb(newLang, oldLang));
    }

    subscribe(callback) {
        this._subscribers.push(callback);
    }
}

const appState = new AppState();
let fromSelect, toSelect, datePicker;

// Инициализация приложения
function init() {
    initDatePicker();
    initSelects();
    setupEventListeners();
    appState.subscribe(handleLanguageChange);
    setDefaultValues();
    updateTranslations(appState.lang);
}

function setupEventListeners() {
    document.querySelector('.lang-switch').addEventListener('click', () => {
        appState.lang = appState.lang === 'ua' ? 'en' : 'ua';
    });
}

function initDatePicker() {
    datePicker = flatpickr("#date", {
        locale: appState.lang === 'ua' ? 'uk' : 'en',
        dateFormat: "Y-m-d",
        defaultDate: new Date(),
        maxDate: new Date(),
        static: true // Фикс для сохранения позиции
    });
}

function initSelects() {
    const commonConfig = {
        searchEnabled: true,
        shouldSort: false,
        duplicateItemsAllowed: false,
        classNames: {
            containerInner: 'choices__inner',
            listDropdown: 'choices__list--dropdown',
            itemSelectable: 'choices__item--selectable'
        },
        searchPlaceholderValue: TRANSLATIONS[appState.lang].searchPlaceholder,
        loadingText: TRANSLATIONS[appState.lang].loading
    };

    fromSelect = new Choices('#fromCurrency', {
        ...commonConfig,
        choices: generateChoiceList(appState.lang),
        placeholderValue: TRANSLATIONS[appState.lang].fromPlaceholder
    });

    toSelect = new Choices('#toCurrency', {
        ...commonConfig,
        choices: generateChoiceList(appState.lang),
        placeholderValue: TRANSLATIONS[appState.lang].toPlaceholder
    });
}


function generateChoiceList(lang) {
    return CURRENCIES
        .map(currency => {
            const translation = TRANSLATIONS[lang]?.currencies?.[currency];
            if (!translation) {
                console.warn(`Missing translation for ${currency} in ${lang}`);
                return null;
            }
            return {
                value: currency,
                label: `${currency} - ${translation}`
            };
        })
        .filter(Boolean);
}

// Улучшенная обработка изменения языка
function handleLanguageChange(newLang) {
    const currentDate = datePicker.selectedDates[0];

    // Обновление календаря
    datePicker.set('locale', newLang === 'ua' ? 'uk' : 'en');
    datePicker.setDate(currentDate || new Date());

    // Плавное обновление селектов
    [fromSelect, toSelect].forEach(select => {
        const tempValue = select.getValue();
        select.destroy();
        select.init();
        select.setChoiceByValue(tempValue);
    });

    // Обновление текстовых элементов
    updateTranslations(newLang);

    // Форсированное обновление размеров
    setTimeout(() => window.dispatchEvent(new Event('resize')), 50);
}

function updateTranslations(lang) {
    // Обновляем статические тексты
    document.querySelectorAll('[data-i18n]').forEach(el => {
        const key = el.dataset.i18n;
        el.textContent = TRANSLATIONS[lang][key] || el.textContent;
    });

    // Обновляем плейсхолдеры
    document.querySelectorAll('[data-i18n-pholder]').forEach(el => {
        const key = el.dataset.i18nPholder;
        el.placeholder = TRANSLATIONS[lang][key] || el.placeholder;
    });
}

function setDefaultValues() {
    if (!fromSelect.getValue()) fromSelect.setChoiceByValue('USD');
    if (!toSelect.getValue()) toSelect.setChoiceByValue('UAH');
}

async function convert() {
    const amount = Number(document.getElementById('amount').value);
    const resultElement = document.getElementById('result');
    const errorElement = document.getElementById('error');

    // Сброс предыдущих сообщений
    errorElement.textContent = '';
    resultElement.innerHTML = '<div class="loader"></div>'; // Показ лоадера

    try {
        // Валидация ввода
        // if (isNaN(amount) throw new Error(TRANSLATIONS[appState.lang].invalidAmount);
        if (amount <= 0) throw new Error(TRANSLATIONS[appState.lang].invalidAmount);

        // Получение кодов валют
        const fromCurrency = fromSelect.getValue(true);
        const toCurrency = toSelect.getValue(true);

        // Проверка валют
        if (!fromCurrency || !toCurrency) {
            throw new Error(TRANSLATIONS[appState.lang].invalidCurrency);
        }

        // Формирование параметров запроса
        const params = new URLSearchParams({
            from: fromCurrency,
            to: toCurrency,
            amount: amount.toFixed(2),
            date: datePicker.input.value,
            places: 2
        });

        // Выполнение запроса с таймаутом
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 10000); // Таймаут 10 сек

        const response = await fetch(`https://194.12.66.70:11000/convert?${params}`, {
            signal: controller.signal
        });

        clearTimeout(timeoutId); // Отмена таймаута при успешном ответе

        // Обработка HTTP-ошибок
        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(`${TRANSLATIONS[appState.lang].error} ${response.status}: ${errorText}`);
        }

        // Парсинг JSON
        const data = await response.json();

        // Проверка структуры ответа
        if (typeof data.result !== 'number') {
            throw new Error(TRANSLATIONS[appState.lang].invalidResponse);
        }

        // Форматирование результата
        resultElement.innerHTML = `
            <div class="result-block">
                <span class="amount">${amount.toFixed(2)} ${fromCurrency}</span>
                <span class="equals">=</span>
                <span class="converted">${data.result.toFixed(2)} ${toCurrency}</span>
            </div>
        `;

    } catch (error) {
        // Обработка ошибок
        errorElement.textContent = error.message.includes("aborted")
            ? TRANSLATIONS[appState.lang].timeoutError
            : error.message;

        resultElement.innerHTML = ''; // Очистка результата при ошибке
        console.error("Ошибка конвертации:", error);

    } finally {
        // Убедиться, что лоадер скрыт даже при успешном выполнении
        const loader = resultElement.querySelector('.loader');
        if (loader) loader.remove();
    }
}


window.convert = convert;
document.addEventListener('DOMContentLoaded', init);