"use client";

import React from "react";
import { motion } from "framer-motion";
import { clsx, type ClassValue } from "clsx";
import { twMerge } from "tailwind-merge";

function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}

export interface DrishyamNode {
  type: string;
  label?: string;
  color?: string;
  source?: string;
  pos?: number[];
  children?: DrishyamNode[];
}

const colorMap: Record<string, string> = {
  "रक्तवर्णः": "bg-red-500 text-white",
  "नीलवर्णः": "bg-blue-600 text-white",
  "श्वेतवर्णः": "bg-white text-black border border-gray-200",
  "हरितवर्णः": "bg-green-500 text-white",
  "पीतवर्णः": "bg-yellow-400 text-black",
  "कृष्णवर्णः": "bg-black text-white",
  "None": "",
};

const textColorMap: Record<string, string> = {
  "रक्तवर्णः": "text-red-500",
  "नीलवर्णः": "text-blue-600",
  "श्वेतवर्णः": "text-white",
  "कृष्णवर्णः": "text-black",
};

export const SanskritRenderer: React.FC<{ node: DrishyamNode }> = ({ node }) => {
  const renderChild = (child: DrishyamNode, index: number) => (
    <SanskritRenderer key={`${child.type}-${index}`} node={child} />
  );

  const containerVariants = {
    hidden: { opacity: 0, y: 20 },
    visible: { opacity: 1, y: 0, transition: { duration: 0.5 } },
  };

  switch (node.type.toLowerCase()) {
    case "box":
    case "मंजूषा":
      return (
        <motion.div
          variants={containerVariants}
          initial="hidden"
          animate="visible"
          className={cn(
            "p-6 rounded-2xl shadow-xl flex flex-col gap-4 m-2",
            colorMap[node.color || "None"] || "bg-white/10 backdrop-blur-md border border-white/20"
          )}
        >
          {node.children?.map(renderChild)}
        </motion.div>
      );

    case "button":
    case "बटनम्":
      return (
        <motion.button
          whileHover={{ scale: 1.05 }}
          whileTap={{ scale: 0.95 }}
          className={cn(
            "px-6 py-3 rounded-xl font-bold transition-all shadow-lg hover:shadow-2xl",
            colorMap[node.color || "None"] || "bg-indigo-600 text-white"
          )}
        >
          {node.label}
        </motion.button>
      );

    case "text":
    case "पाठः":
      return (
        <motion.p
          className={cn(
            "text-lg font-medium",
            textColorMap[node.color || "None"] || "text-gray-200"
          )}
        >
          {node.label}
        </motion.p>
      );

    case "image":
    case "चित्त्रम्":
      return (
        <motion.img
          src={node.source || "/placeholder.png"}
          alt={node.label || "Sanskrit UI Image"}
          className="rounded-xl object-cover shadow-lg w-full max-w-md h-auto"
        />
      );

    case "input":
    case "प्रविष्टिः":
      return (
        <input
          placeholder={node.label || "लिखतु..."}
          className="px-4 py-3 rounded-lg bg-white/5 border border-white/20 text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"
        />
      );

    case "list":
    case "सूची":
      return (
        <div className="flex flex-col gap-2 w-full">
          {node.children?.map((item, i) => (
            <motion.div
              key={i}
              className="p-4 bg-white/5 rounded-lg border border-white/10 hover:bg-white/10 transition-all cursor-pointer"
            >
              {renderChild(item, i)}
            </motion.div>
          ))}
        </div>
      );

    default:
      return <div className="text-red-500 font-mono">Unknown Component: {node.type}</div>;
  }
};
