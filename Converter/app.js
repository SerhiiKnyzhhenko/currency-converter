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
    fromSelect = new Choices('#fromCurrency', {
        choices: generateChoiceList(appState.lang),
        searchEnabled: true,
        shouldSort: true,
        duplicateItemsAllowed: false,
        classNames: { listDropdown: 'choices__list--dropdown' }
    });

    toSelect = new Choices('#toCurrency', {
        choices: generateChoiceList(appState.lang),
        searchEnabled: true,
        shouldSort: true,
        duplicateItemsAllowed: false,
        classNames: { listDropdown: 'choices__list--dropdown' }
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

function handleLanguageChange(newLang, oldLang) {
    // Сохраняем выбранные значения и дату
    const selectedFrom = fromSelect.getValue();
    const selectedTo = toSelect.getValue();
    const currentDate = datePicker.selectedDates[0];

    // Обновляем DatePicker без пересоздания
    datePicker.set('locale', newLang === 'ua' ? 'uk' : 'en');
    datePicker.setDate(currentDate || new Date());

    // Обновляем селекты с сохранением значений
    [fromSelect, toSelect].forEach(select => select.destroy());

    initSelects();

    // Восстанавливаем значения после инициализации
    setTimeout(() => {
        fromSelect.setChoiceByValue(selectedFrom);
        toSelect.setChoiceByValue(selectedTo);
        if (!selectedFrom) fromSelect.setChoiceByValue('USD');
        if (!selectedTo) toSelect.setChoiceByValue('UAH');
    }, 100);

    // Обновляем тексты
    updateTranslations(newLang);
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
    const amountElement = document.getElementById('amount');
    const resultElement = document.getElementById('result');
    const errorElement = document.getElementById('error');
    const API_URL = 'https://194.12.66.70:11000/api/convert';

    errorElement.textContent = '';
    resultElement.textContent = '';

    try {
        const amount = parseFloat(amountElement.value);
        if (isNaN(amount) || amount <= 0) {
            throw new Error(TRANSLATIONS[appState.lang].invalidAmount);
        }

        const params = new URLSearchParams({
            from: fromSelect.getValue(),
            to: toSelect.getValue(),
            amount: amount,
            date: datePicker.input.value
        });

        const response = await fetch(`${API_URL}?${params}`);
        if (!response.ok) {
            throw new Error(`${TRANSLATIONS[appState.lang].error} ${response.status}`);
        }

        const data = await response.json();
        if (typeof data.result !== 'number') {
            throw new Error(TRANSLATIONS[appState.lang].invalidResponse);
        }

        resultElement.textContent =
            `${amount} ${fromSelect.getValue()} = ${data.result.toFixed(2)} ${toSelect.getValue()}`;

    } catch (error) {
        errorElement.textContent = error.message;
        console.error('Conversion error:', error);
    }
}

window.convert = convert;
document.addEventListener('DOMContentLoaded', init);