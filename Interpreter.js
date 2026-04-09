class Interpreter {
    constructor() {
        this.environment = new Map();
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
                this.environment.set(stmt.id, val);
                break;
            case 'PrintStatement':
                console.log(this.evaluate(stmt.expression));
                break;
            case 'ExpressionStatement':
                this.evaluate(stmt.expression);
                break;
            case 'IfStatement':
                if (this.evaluate(stmt.condition)) {
                    this.executeBlockOrStatement(stmt.consequence);
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
                if (this.environment.has(expr.name)) {
                    return this.environment.get(expr.name);
                }
                throw new Error(`Undefined variable: ${expr.name}`);
            case 'Assignment':
                let value = this.evaluate(expr.right);
                // Allow assignment to implicitly declare if not present in loops, but strict generally
                // Actually, for the loop like `i = 1`, if it's not declared with अस्ति, we should allow it as implicit declaration or simply set it.
                this.environment.set(expr.left, value);
                return value;
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
