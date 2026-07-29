const Environment = class {
    constructor(parent = null) {
        this.values = new Map();
        this.constants = new Set();
        this.parent = parent;
    }
};
console.log("Environment class created.");
