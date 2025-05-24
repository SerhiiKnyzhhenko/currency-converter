// app.js
import { CURRENCIES, TRANSLATIONS } from './currencies-generated.js';

let currentLang = 'ua';
const API_URL = 'https://194.12.66.70:11000/api/convert';

// Инициализация
function init() {
    populateCurrencies();
    updateTranslations();
}

// Заполнение списка валют
function populateCurrencies() {
    const from = document.getElementById('fromCurrency');
    const to = document.getElementById('toCurrency');

    from.innerHTML = '';
    to.innerHTML = '';

    CURRENCIES.forEach(currency => {
        const text = `${currency} - ${TRANSLATIONS[currentLang].currencies[currency]}`;
        from.add(new Option(text, currency));
        to.add(new Option(text, currency));
    });
}

// Переключение языка
function toggleLanguage() {
    currentLang = currentLang === 'ua' ? 'en' : 'ua';
    populateCurrencies();
    updateTranslations();
}

// Обновление текстов
function updateTranslations() {
    document.querySelectorAll('[data-i18n]').forEach(el => {
        el.textContent = TRANSLATIONS[currentLang][el.dataset.i18n];
    });

    document.querySelectorAll('[data-i18n-pholder]').forEach(el => {
        el.placeholder = TRANSLATIONS[currentLang][el.dataset.i18nPholder];
    });
}

// Конвертация
async function convert() {
    const amount = document.getElementById('amount').value;
    const from = document.getElementById('fromCurrency').value;
    const to = document.getElementById('toCurrency').value;

    try {
        const response = await fetch(`${API_URL}?from=${from}&to=${to}&amount=${amount}`);
        const data = await response.json();

        document.getElementById('result').textContent =
            `${amount} ${from} = ${data.result} ${to}`;
    } catch (error) {
        document.getElementById('error').textContent =
            TRANSLATIONS[currentLang].error + error.message;
    }
}

// Экспорт функций в глобальную область видимости
window.toggleLanguage = toggleLanguage;
window.convert = convert;

// Запуск при загрузке
init();