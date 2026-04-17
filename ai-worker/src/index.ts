import { Env } from './errorHandler';
import { generateAIContent } from './aiLogic';

export default {
  async fetch(request: Request, env: Env, ctx: ExecutionContext): Promise<Response> {
    const url = new URL(request.url);

    if (request.method === "POST" && url.pathname === "/ai/compile") {
      try {
        const body = await request.json() as { code: string };
        const code = body.code;
        if (!code) {
          return new Response(JSON.stringify({ error: "No code provided" }), { status: 400, headers: { 'Content-Type': 'application/json' } });
        }

        const prompt = `Analyze this code, parse it, identify any issues, and give output that can execute it:\n\n${code}`;
        const responseJsonStr = await generateAIContent(prompt, env);

        return new Response(responseJsonStr, {
          headers: { 'Content-Type': 'application/json' }
        });
      } catch (e: any) {
        return new Response(JSON.stringify({ error: e.message }), { status: 500, headers: { 'Content-Type': 'application/json' } });
      }
    }

    return new Response("Nava AI Gateway Active", { status: 200 });
  }
};
