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

    errorElement.textContent = '';
    resultElement.innerHTML = '<div class="loader"></div>';

    try {
        // Валидация ввода
        if (!amount || amount <= 0) {
            throw new Error(TRANSLATIONS[appState.lang].invalidAmount);
        }

        // Запрос к API
        const response = await fetch(`https://api.exchangerate.host/convert?${new URLSearchParams({
            from: fromSelect.getValue(),
            to: toSelect.getValue(),
            amount: amount,
            date: datePicker.input.value,
            places: 2
        })}`);

        if (!response.ok) throw new Error(TRANSLATIONS[appState.lang].networkError);

        const data = await response.json();
        if (data.error) throw new Error(data.error);

        // Форматирование результата
        resultElement.textContent = new Intl.NumberFormat(appState.lang === 'ua' ? 'uk-UA' : 'en-US', {
            style: 'currency',
            currency: toSelect.getValue()
        }).format(data.result);

    } catch (error) {
        errorElement.textContent = error.message;
        console.error('Conversion error:', error);
    } finally {
        resultElement.innerHTML = '';
    }
}


window.convert = convert;
document.addEventListener('DOMContentLoaded', init);