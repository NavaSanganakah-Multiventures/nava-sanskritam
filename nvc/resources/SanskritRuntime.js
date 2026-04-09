/**
 * SUL v19.0 - Sanskrit Universal Web Runtime (Vanilla JS)
 * This script renders declarative JSON from the NVC compiler into standard DOM.
 */

const colorMap = {
    "रक्तवर्णः": "bg-red-500 text-white",
    "नीलवर्णः": "bg-blue-600 text-white",
    "श्वेतवर्णः": "bg-white text-black border border-gray-200",
    "हरितवर्णः": "bg-green-500 text-white",
    "पीतवर्णः": "bg-yellow-400 text-black",
    "कृष्णवर्णः": "bg-black text-white",
    "None": ""
};

const textColorMap = {
    "रक्तवर्णः": "text-red-500",
    "नीलवर्णः": "text-blue-600",
    "श्वेतवर्णः": "text-white",
    "कृष्णवर्णः": "text-black"
};

function createDrishyam(node) {
    const type = node.type.toLowerCase();
    let el;

    if (type === "box" || type === "मंजूषा") {
        el = document.createElement("div");
        el.className = `p-6 rounded-2xl shadow-xl flex flex-col gap-4 m-2 transition-all duration-500 ${colorMap[node.color] || "bg-white/10 backdrop-blur-md border border-white/20"}`;
    } else if (type === "button" || type === "बटनम्") {
        el = document.createElement("button");
        el.innerText = node.label;
        el.className = `px-6 py-3 rounded-xl font-bold transition-all shadow-lg hover:shadow-2xl hover:scale-105 active:scale-95 ${colorMap[node.color] || "bg-indigo-600 text-white"}`;
    } else if (type === "text" || type === "पाठः") {
        el = document.createElement("p");
        el.innerText = node.label;
        el.className = `text-lg font-medium ${textColorMap[node.color] || "text-gray-200"}`;
    } else if (type === "image" || type === "चित्त्रम्") {
        el = document.createElement("img");
        el.src = node.source || "https://api.placeholder.com/150";
        el.className = "rounded-xl object-cover shadow-lg w-full max-w-md h-auto";
    } else if (type === "input" || type === "प्रविष्टिः") {
        el = document.createElement("input");
        el.placeholder = node.label || "लिखतु...";
        el.className = "px-4 py-3 rounded-lg bg-white/5 border border-white/20 text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all";
    } else if (type === "list" || type === "सूची") {
        el = document.createElement("div");
        el.className = "flex flex-col gap-2 w-full";
        // Children handled below
    } else {
        el = document.createElement("div");
        el.innerText = "Unknown: " + node.type;
        el.className = "text-red-500";
    }

    if (node.children && node.children.length > 0) {
        node.children.forEach(child => {
            if (type === "list" || type === "सूची") {
                const wrapper = document.createElement("div");
                wrapper.className = "p-4 bg-white/5 rounded-lg border border-white/10 hover:bg-white/10 transition-all cursor-pointer";
                wrapper.appendChild(createDrishyam(child));
                el.appendChild(wrapper);
            } else {
                el.appendChild(createDrishyam(child));
            }
        });
    }

    return el;
}

async function initSUL() {
    try {
        const response = await fetch('darshanam.json');
        const data = await response.json();
        const root = document.getElementById('sul-root');
        
        data.forEach(darshanam => {
            const container = document.createElement("div");
            container.id = darshanam.id;
            container.className = "w-full max-w-4xl mx-auto py-10";
            
            darshanam.elements.forEach(elem => {
                container.appendChild(createDrishyam(elem));
            });
            root.appendChild(container);
        });
    } catch (err) {
        console.error("SUL Runtime Error:", err);
    }
}

window.onload = initSUL;
