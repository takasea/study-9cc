typedef struct Token Token;
typedef struct Node Node;
typedef struct LVar LVar;

extern char *user_input;
extern Token *token;
extern Node *code[100];


extern Token *tokenize(char *p);
extern void program();

extern int count_lvar();
extern void gen(Node *node);


// トークンの種類
typedef enum
{
    TK_RESERVED,    // 記号
    TK_INDENT,      // 識別子
    TK_NUM,         // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
} TokenKind;


// トークン型
struct Token
{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        //  トークンの長さ
};


// 抽象構文木のノードの種類
typedef enum{
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_NUM,     // 整数
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LT,      // > <
    ND_LE,      // <= >=
    ND_ASSIGN,  // =
    ND_LVAR,    // ローカル変数
    ND_RETURN,  // return
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCTION,
} NodeKind;


// 抽象構文木のノードの型
struct Node{
    NodeKind kind;  // ノードの型
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
    char *label;    // kindがND_FUNCTIONの場合のみ使う
    int fn_len;     // kindがND_FUNCTIONの場合のみ使う
    Node *block[100];    // kindがND_BLOCKの場合のみ使う ブロックに含まれる式
};


//ローカル変数の型
struct LVar{
    LVar *next; //次の変数がNULL
    char *name; //変数の名前
    int len;    //名前の長さ
    int offset; //RBPからのオフセット
};

