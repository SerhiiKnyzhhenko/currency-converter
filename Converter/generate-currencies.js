// generate-currencies.js
const fs = require('fs');
const currencies = require('./currencies-data');

// Генерация массива кодов валют
const currencyCodes = Object.keys(currencies);

// Генерация объекта переводов
const translations = {
    en: { currencies: {} },
    ua: { currencies: {} }
};

for (const [code, data] of Object.entries(currencies)) {
    translations.en.currencies[code] = data.en;
    translations.ua.currencies[code] = data.ua;
}

// Запись в файл
const output = `// auto-generated file
export const CURRENCIES = ${JSON.stringify(currencyCodes, null, 4)};

export const TRANSLATIONS = ${JSON.stringify({
    en: {
        title: "Online Currency Converter",
        amount: "Amount",
        convert: "Convert",
        error: "Error:",
        currencies: translations.en.currencies
    },
    ua: {
        title: "Онлайн конвертер валют",
        amount: "Сума",
        convert: "Конвертувати",
        error: "Помилка:",
        currencies: translations.ua.currencies
    }
}, null, 4)};`;

fs.writeFileSync('./currencies-generated.js', output);