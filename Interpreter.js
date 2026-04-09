const NavaDarshakam = require('./NavaDarshakam');

class Environment {
    constructor(parent = null) {
        this.values = new Map();
        this.constants = new Set();
        this.parent = parent;
    }

    define(name, value, isConstant = false) {
        if (this.values.has(name)) {
            throw new Error(`Variable '${name}' is already defined.`);
        }
        this.values.set(name, value);
        if (isConstant) {
            this.constants.add(name);
        }
    }

    get(name) {
        if (this.values.has(name)) {
            return this.values.get(name);
        }
        if (this.parent !== null) {
            return this.parent.get(name);
        }
        throw new Error(`Undefined variable: ${name}`);
    }

    assign(name, value) {
        if (this.values.has(name)) {
            if (this.constants.has(name)) {
                throw new Error(`Cannot reassign constant '${name}'.`);
            }
            this.values.set(name, value);
            return;
        }
        if (this.parent !== null) {
            this.parent.assign(name, value);
            return;
        }
        // Implicitly declare if not found anywhere (mostly for backward compatibility in loops)
        this.values.set(name, value);
    }
}

class Interpreter {

    constructor() {
        this.environment = new Environment();
        this.darshakam = new NavaDarshakam();

        // Built-in functions
        this.environment.define('समय', {
            type: 'NativeFunction',
            call: () => Date.now()
        }, true);

        this.environment.define('गणन', {
            type: 'NativeFunction',
            call: (a, b) => {
                if (typeof a === 'number' && typeof b === 'number') return a + b;
                return 0;
            }
        }, true);

        // Vedic UI Syntax
        const tags = ['अङ्गम्', 'शीर्षकम्', 'अनुच्छेदः', 'सूची', 'बटनम्'];
        for (let tag of tags) {
            this.environment.define(tag, {
                type: 'NativeFunction',
                call: (content, styleObj = null) => {
                    this.darshakam.addTag(tag, content, styleObj);
                    return null;
                }
            }, true);
        }
    }

    interpret(ast) {
        if (ast.type === 'Program') {
            for (let stmt of ast.body) {
                this.execute(stmt);
            }
        } else {
            throw new Error(`Unknown AST node type: ${ast.type}`);
        }
    }

    execute(stmt) {
        switch (stmt.type) {
            case 'VariableDeclaration':
                let val = this.evaluate(stmt.init);
                this.environment.define(stmt.id, val, false);
                break;
            case 'ConstantDeclaration':
                let constVal = this.evaluate(stmt.init);
                this.environment.define(stmt.id, constVal, true);
                break;
            case 'FunctionDeclaration':
                this.environment.define(stmt.id.name, stmt, false);
                break;
            case 'ReturnStatement':
                let retVal = null;
                if (stmt.argument) {
                    retVal = this.evaluate(stmt.argument);
                }
                throw { type: 'ReturnValue', value: retVal };
            case 'PrintStatement':
                console.log(this.evaluate(stmt.expression));
                break;
            case 'ExpressionStatement':
                this.evaluate(stmt.expression);
                break;
            case 'IfStatement':
                if (this.evaluate(stmt.condition)) {
                    this.executeBlockOrStatement(stmt.consequence);
                } else if (stmt.alternate) {
                    this.executeBlockOrStatement(stmt.alternate);
                }
                break;
            case 'LoopStatement':
                if (stmt.init) {
                    this.evaluate(stmt.init);
                }
                while (stmt.test ? this.evaluate(stmt.test) : true) {
                    this.executeBlockOrStatement(stmt.body);
                    if (stmt.update) {
                        this.evaluate(stmt.update);
                    }
                }
                break;
            case 'BlockStatement':
                for (let bStmt of stmt.body) {
                    this.execute(bStmt);
                }
                break;
            default:
                throw new Error(`Unknown statement type: ${stmt.type}`);
        }
    }

    executeBlockOrStatement(node) {
        if (node.type === 'BlockStatement') {
            this.execute(node);
        } else {
            this.execute(node);
        }
    }

    evaluate(expr) {
        switch (expr.type) {
            case 'Literal':
                return expr.value;
            case 'Identifier':
                return this.environment.get(expr.name);
            case 'CallExpression':
                let callee = this.evaluate(expr.callee);
                let args = expr.arguments.map(arg => this.evaluate(arg));

                if (callee.type === 'NativeFunction') {
                    return callee.call(...args);
                }

                if (callee.type === 'FunctionDeclaration') {
                    let previousEnv = this.environment;
                    let localEnv = new Environment(this.environment);

                    for (let i = 0; i < callee.params.length; i++) {
                        let paramName = callee.params[i];
                        let argValue = i < args.length ? args[i] : null;
                        localEnv.define(paramName, argValue, false);
                    }

                    this.environment = localEnv;
                    try {
                        this.executeBlockOrStatement(callee.body);
                    } catch (e) {
                        if (e.type === 'ReturnValue') {
                            this.environment = previousEnv;
                            return e.value;
                        }
                        throw e;
                    }
                    this.environment = previousEnv;
                    return null;
                }

                throw new Error("Cannot call non-function.");
            case 'Assignment':
                let value = this.evaluate(expr.right);
                this.environment.assign(expr.left, value);
                return value;
            case 'ObjectLiteral':
                let obj = {};
                for (let key in expr.properties) {
                    obj[key] = this.evaluate(expr.properties[key]);
                }
                return obj;
            case 'MemberAccess':
                let target = this.evaluate(expr.object);
                if (target && typeof target === 'object') {
                    return target[expr.property];
                }
                throw new Error(`Cannot access property '${expr.property}' of non-object.`);
            case 'BinaryExpression':
                let left = this.evaluate(expr.left);
                let right = this.evaluate(expr.right);
                switch (expr.operator) {
                    case '+': return left + right;
                    case '-': return left - right;
                    case '*': return left * right;
                    case '/': return left / right;
                    case '<': return left < right;
                    case '<=': return left <= right;
                    case '>': return left > right;
                    case '>=': return left >= right;
                    case '==': return left === right;
                    case '!=': return left !== right;
                    default:
                        throw new Error(`Unknown operator: ${expr.operator}`);
                }
            default:
                throw new Error(`Unknown expression type: ${expr.type}`);
        }
    }
}

module.exports = Interpreter;
