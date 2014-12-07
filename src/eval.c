// vim: noet ts=4 sw=4
#include <assert.h>
#include <string.h>
#include "eval.h"
#include "lair_std.h"
#include "map.h"
#include "parse.h"

_lair_env *_lair_standard_env() {
	_lair_env *std_env = calloc(1, sizeof(_lair_env));

	assert(_lair_add_function(std_env, "+", 2, &_lair_builtin_operator_plus) != NULL);
	assert(_lair_add_function(std_env, "print", 1, &_lair_builtin_print) != NULL);

	return std_env;
}

_lair_function *
_lair_add_function(_lair_env *env,
		const char *name,
		const int argc,
		struct _lair_type *(*func_ptr)(LAIR_FUNCTION_SIG)) {
	assert(name != NULL);
	assert(env != NULL);
	assert(strlen(name) > 0);

	/* Check to see if that function already exists: */
	const _lair_function *existing_func = _tst_map_get(env->c_functions, name, strlen(name));
	if (existing_func != NULL)
		return NULL;

	_lair_function _stack_func = {
		.argc = argc,
		.argv = calloc(argc, sizeof(_lair_type)),
		.function_ptr = func_ptr
	};
	_lair_function *new_func = calloc(1, sizeof(_lair_function));
	memcpy(new_func, &_stack_func, sizeof(_lair_function));

	_tst_map_insert(&(env->c_functions), name, strlen(name), new_func, sizeof(_lair_function));

	return new_func;

}

static _lair_type *_lair_call_builtin(const _lair_ast *ast_node, _lair_env *env, const _lair_function *builtin_function) {
	if (ast_node->next->atom.type == LR_CALL) {
		/* We need to evaluate the RHS before we can pass it to the function
		 * as arguments.
		 */
		const _lair_type *args[1];
		args[0] = _lair_env_eval(ast_node->next, env);
		return builtin_function->function_ptr(builtin_function->argc, args);
	} else {
		assert(1 == 0 && "Not yet implemented.");
		//_lair_ast *cur_node = ast_node->next;
		//int i;
		//for (i = 0; i < builtin_function->argc; i++) {
		//}
	}

	return NULL;
}

static _lair_type *_lair_call_function(const _lair_ast *ast_node, _lair_env *env) {
	/* Determine if the thing we're trying to call is a function
	 * or not. It might be an atom, in which case we need to check
	 * or function/c_function maps to see if it's in there.
	 */
	const char *func_name = ast_node->atom.value.str;
	const size_t func_len = strlen(ast_node->atom.value.str);

	const _lair_function *builtin_function = _tst_map_get(env->c_functions, func_name, func_len);
	if (builtin_function != NULL)
		return _lair_call_builtin(ast_node, env, builtin_function);

	const _lair_ast *defined_function_ast = _tst_map_get(env->functions, func_name, func_len);
	assert(defined_function_ast != NULL && "No such function to call.");

	/* TODO: Check the environment for non-C functions as well. */
	return NULL;
}

const _lair_type *_lair_env_eval(const struct _lair_ast *ast, _lair_env *env) {
	const _lair_ast *cur_ast_node = ast;
	switch (ast->atom.type) {
		case LR_FUNCTION:
			return _lair_call_function(cur_ast_node, env);
		case LR_CALL:
			return _lair_call_function(cur_ast_node->next, env);
		case LR_ATOM:
			/* TODO: See if the value of this ATOM is actually a function.
			 * Then eval that.
			 */
		default:
			return &ast->atom;
	}
	/* TODO: Expire anything in this scope. */

	return NULL;
}

int _lair_eval(const struct _lair_ast *root) {
	_lair_env *std_env = _lair_standard_env();
	const _lair_ast *cur_ast_node = root->children;

	while (cur_ast_node != NULL) {
		if (cur_ast_node->atom.type == LR_CALL) {
			_lair_env_eval(cur_ast_node, std_env);
		} else if (cur_ast_node->atom.type == LR_FUNCTION) {
			/* TODO: Add functions defined in code to the env. */
		}

		cur_ast_node = cur_ast_node->sibling;
	}

	_lair_free_env(std_env);
	return 0;
}

void _lair_free_env(_lair_env *env) {
	free(env);
}
