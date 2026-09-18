# React frontend

This directory contains the read-only production-style dashboard for the Quant Market Microstructure Lab.

It is intentionally built with **React + Vite**, not Next.js. It can be deployed as a static site to Vercel, Netlify, GitHub Pages, Cloudflare Pages, or any static host.

## Local development

```sh
cd frontend
npm install
npm run dev
```

Open `http://127.0.0.1:5173`.

## Production build

```sh
npm run build
npm run preview
```

The deployable static files are generated in `frontend/dist/`.

## Configuration

The dashboard works with local fixture data by default. If `/api/report` is available on the same origin, the dashboard reads it automatically and falls back safely to the embedded read-only report when the API is unavailable.

No trading credentials are accepted or stored in the frontend.
