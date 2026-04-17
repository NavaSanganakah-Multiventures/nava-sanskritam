# Nava AI Worker

यह Cloudflare Worker है जो Nava Sanskritam के लिए AI Gateway के साथ बातचीत करता है।

## उद्देश्य (Purpose)
यह Worker `.ns` (Nava Sanskritam) कोड को स्वीकार करता है, उसे Cloudflare AI Gateway के माध्यम से प्रोसेस करता है, और निष्पादन योग्य आउटपुट (executable output) वापस करता है।

## सेट-अप (Setup)
1. `npm install` चलाएँ
2. अपने Cloudflare Account ID और API Tokens को `SECRETS_KV` में सेट करें:
   - `CLOUDFLARE_ACCOUNT_ID`
   - `CLOUDFLARE_API_TOKEN`
   - `CF_AIG_TOKEN` (वैकल्पिक)
   - `AI_GATEWAY_ID` (डिफ़ॉल्ट "vertaxai")
3. `npm run deploy` का उपयोग करके डिप्लॉय करें।
