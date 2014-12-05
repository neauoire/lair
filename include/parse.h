// vim: noet ts=4 sw=4
#pragma once
/** @file
 * This file is not intended to be used outside of lair, but can be useful
 * when debugging or just for digging around in. It is documented to avoid
 * confusion.
 */

/**
 * @brief	Token types use when parsing.
 */
typedef enum {
	LR_ERR, /**	Unknown type. Should not happen. */
	LR_FUNCTION, /**	Function. */
	LR_OPERATOR, /**	An operator (+, -, ?, etc.). */
	LR_RETURN, /**	The return built-in function. */
	LR_FUNCTION_ARG, /**	A parameter to a function. */
	LR_VARIABLE, /**	A variable. */
	LR_INDENT, /**	An indent. */
	LR_DEDENT, /**	A dedent. Used on every newline. */
	LR_EOF, /**	EOF token. */
	LR_STRING, /**	A string constant. */
	LR_CALL, /**	A function call. */
	LR_ATOM, /**	Atomic symbol. Reference to either a variable or a function. */
	LR_NUM /**	A number. */
} LAIR_TOKEN;

/**
 * @brief	Simple string type.
 */
typedef struct _str {
	const size_t size; /**	The size of the string, in bytes. */
	const char *data; /**	The actual string content. */
} _str;

/**
 * @brief	This is a representation of a token use for parsing into the AST later.
 */
typedef struct _lair_token {
	char *token_str; /**	The string representation of the token. Is probably NUL-terminated. */
	LAIR_TOKEN token_type; /**	The enumerated token's type. */
	unsigned int indent_level; /**	The indent level for this piece of code. */
	struct _lair_token *next; /**	Link to the next token in the list. */
	struct _lair_token *prev; /**	Pointer to the previous token in the list. */
} _lair_token;

/**
 * @brief	Representation of a value. This is how we hold variable information.
 */
typedef union _lair_value {
	int num; /**	If this type is an integer, this will be the integer value. */
	char *str; /**	Like `num`, but this will hold a string instead. */
} _lair_value;

/**
 * @brief	Meta-information about a `_lair_value`.
 */
typedef struct _lair_type {
	const LAIR_TOKEN type; /**	This lets us know what kind of type the value is. Number, string, etc. */
	_lair_value value; /**	The actual value. */
} _lair_type;

/**
 * @brief	The main struct for representing the AST.
 * This is basically a list with lists in it. Each node can have children.
 */
typedef struct _lair_ast {
	struct _lair_ast *next; /**	Next item in the list. */
	struct _lair_ast *children; /**	Child lists. */
	struct _lair_ast *sibling; /** If this list is a child of something else, this pointer will be this item's sibling. */
	_lair_type atom; /**	The value of this AST object. */
} _lair_ast;

/**
 * Parses a raw program (string) into tokens.
 * @param[in]	program	The program to be parsed.
 * @param[in]	len	The length of the program, in bytes.
 */
_lair_token *_lair_tokenize(const char *program, const size_t len);

/**
 * Helper function that dumps `_lair_token`s to STDOUT in a human
 * friendly format.
 * @param[in]	tokens	Tokens to print.
 */
void lair_print_tokens(const _lair_token *tokens);

/**
 * Frees the tokens generated by `_lair_tokenize`.
 * @param[in]	tokens	The tokens generated by `_lair_tokenize`.
 */
void _lair_free_tokens(_lair_token *tokens);

/**
 * Takes a list of tokens and turns it into an AST.
 * @param[in]	tokens	The list of tokens to manipulate.
 */
_lair_ast *_lair_parse_from_tokens(_lair_token **tokens);
