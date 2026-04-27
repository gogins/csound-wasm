import fs from "node:fs";
import path from "node:path";

const repo_root = process.cwd();

const src = path.join(
    repo_root,
    "node_modules",
    "@csound",
    "wasm-bin",
    "lib",
    "csound.wasm"
);

const dst_dir = path.join(repo_root, "web", "csound");
const dst = path.join(dst_dir, "csound.wasm");

fs.mkdirSync(dst_dir, { recursive: true });
fs.copyFileSync(src, dst);

console.log(`Copied ${src}`);
console.log(`To     ${dst}`);