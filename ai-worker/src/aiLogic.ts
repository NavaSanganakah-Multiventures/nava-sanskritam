import { Env } from './errorHandler';

/**
 * Sanitizes the AI response by removing markdown blocks and extracting JSON content.
 */
export function sanitizeJson(text: string): string {
  let sanitized = text.replace(/```json/g, "").replace(/```/g, "").trim();
  const firstBrace = sanitized.indexOf("{");
  const lastBrace = sanitized.lastIndexOf("}");
  if (firstBrace !== -1 && lastBrace !== -1) {
    sanitized = sanitized.substring(firstBrace, lastBrace + 1);
  }
  return sanitized;
}

/**
 * Strict Single-Path AI Gateway Generation.
 * All logic, fallbacks, and model selection are managed on the Cloudflare Dashboard side.
 * This Worker code only acts as a secure requester.
 */
export async function generateAIContent(prompt: string, env: Env): Promise<string> {
  const accountId = await env.SECRETS_KV.get('CLOUDFLARE_ACCOUNT_ID');

  const cfToken = await env.SECRETS_KV.get('CLOUDFLARE_API_TOKEN');
  const aigToken = await env.SECRETS_KV.get('CF_AIG_TOKEN') || cfToken;
  const gatewayId = (await env.SECRETS_KV.get('AI_GATEWAY_ID')) || "vertaxai";

  const model = "dynamic/r";

  if (!accountId || !aigToken || aigToken === "null") {
    throw new Error("AI Setup Incomplete: Missing Cloudflare Credentials.");
  }

  const gatewayUrl = `https://gateway.ai.cloudflare.com/v1/${accountId}/${gatewayId}/compat/chat/completions`;

  try {
    const gRes = await fetch(gatewayUrl, {
      method: 'POST',
      headers: {
        'cf-aig-authorization': `Bearer ${aigToken}`,
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        model: model,
        messages: [
          { role: "system", content: "You are a professional content generation assistant. Write high-quality, engaging, and structured content based on the user's prompt. Output only the requested JSON content." },
          { role: "user", content: prompt }
        ],
        response_format: { type: "json_object" }
      })
    });

    const resText = await gRes.text();

    if (gRes.ok) {
      if (!resText || resText.trim() === "") {
        throw new Error(`Gateway returned 200 OK but EMPTY response for ${model}`);
      }
      const aiResponse = JSON.parse(resText);
      if (aiResponse.choices && aiResponse.choices[0] && aiResponse.choices[0].message) {
        return sanitizeJson(aiResponse.choices[0].message.content);
      }
      throw new Error(`Gateway returned invalid JSON structure for ${model}: ${resText.substring(0, 200)}`);
    } else {
      throw new Error(`Gateway Fetch failed for ${model} (Status: ${gRes.status}): ${resText}`);
    }
  } catch (e: any) {
    throw new Error(`AI Gateway Request Failed: ${e.message}`);
  }
}
