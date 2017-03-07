typedef int bool;
#define true 1
#define false 0

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum token_type {
	OP,
	LPAREN,
	RPAREN,
	UNSIGNED_VAR
} token_type;

bool is_op(char* buff, int buff_length) {
	// All binary- and unary- operators are assumed to be single-byte only.
	if (buff_length != 1) return false;
	char c = (*buff);
	return (c == '+' || c == '-' || c == '*' || c == '/');
}

bool is_left_parenthesis(char* buff, int buff_length) {
	if (buff_length != 1) return false;
	char c = (*buff);
	return (c == '(');
}

bool is_right_parenthesis(char* buff, int buff_length) {
	if (buff_length != 1) return false;
	char c = (*buff);
	return (c == ')');
}

token_type get_token_type(char* buff, int buff_length) {
	if (is_op(buff, buff_length)) {
		return OP;
	} else if (is_left_parenthesis(buff, buff_length)) {
		return LPAREN;
	} else if (is_right_parenthesis(buff, buff_length)) {
		return RPAREN;
	} else {
		return UNSIGNED_VAR;
	}	
}

typedef struct token {
	token_type type;
	char* name;
} token;

token* create_token(char* a_name, int a_name_length) {
	token* ret = (token*) malloc(sizeof(token));
	ret->name = (char*)calloc(a_name_length, sizeof(char)); 
	memcpy(ret->name, a_name, a_name_length * sizeof(char));	
	ret->type = get_token_type(a_name, a_name_length);
	return ret;
}

bool delete_token(token* ref) {
	if (NULL == ref) return true;
	if (ref->name) free(ref->name);
	free(ref);
	return true;	
}

typedef struct token_list_node {
	token* t;
	struct token_list_node* next;
} token_list_node;

token_list_node* create_token_list_node(token* a_token) {
	token_list_node* ret = (token_list_node*) malloc(sizeof(token_list_node));
	ret->t = a_token;
	ret->next = NULL; 
	return ret;
}

bool push_token_list_node(token_list_node* *p_head, token_list_node* *p_tail, token_list_node* node) {
	if (!(*p_head)) {
		(*p_head) = node;
		(*p_tail) = node;
		return true;
	}
	
	(*p_tail)->next = node;
	(*p_tail) = node;
	return true;
}

token_list_node* delete_token_list_node(token_list_node* ref) {
	if (NULL == ref) return NULL;
	if (ref->t) delete_token(ref->t);
	token_list_node* ret = ref->next;
	free(ref);
	return ret;  
} 

bool is_in_byte_delimiters(char c, char* byte_delimiters, int byte_delimiters_size) {
	// TODO: Optimize by `set`.
	for (int i = 0; i < byte_delimiters_size; ++i)
		if (c == (*(byte_delimiters + i))) 
			return true;

	return false;
}

void print_token_list_delimited_by_space(token_list_node* head) {
	token_list_node* it = head;
	while (it) {
		printf("%s ", it->t->name);
		it = it->next;
	}	
	puts("");
}

token_list_node* create_token_list(char* s, int s_length, char* byte_delimiters_to_include, int byte_delimiters_to_include_size, char* byte_delimiters_to_exclude, int byte_delimiters_to_exclude_size) {
	// Padding `s` with any `byte delimiter to exclude` eliminates the necessity to handle cases where `s` doesn't end with a delimiter leaving a non-empty `buff`.
	char* padded_s = (char*) malloc((3 + s_length) * sizeof(char)); 
	padded_s[0] = ' ';
	memcpy(padded_s + 1, s, s_length * sizeof(char));	
	padded_s[1 + s_length] = ' ';
	padded_s[2 + s_length] = '\0';

	char* tmp_ptr = padded_s;		

	token_list_node* head = NULL;
	token_list_node* tail = NULL;

	char buff[1024];
	bzero(buff, sizeof buff);
	while (*tmp_ptr != '\n' && *tmp_ptr != '\0') {
		if (is_in_byte_delimiters(*tmp_ptr, byte_delimiters_to_include, byte_delimiters_to_include_size)) {
			if (strlen(buff) > 0) {
				token* t = create_token(buff, strlen(buff));	
				token_list_node* tn = create_token_list_node(t); 
				push_token_list_node(&head, &tail, tn);
				bzero(buff, sizeof buff);
			}

			token* d = create_token(tmp_ptr, 1);	
			token_list_node* dn = create_token_list_node(d);
			push_token_list_node(&head, &tail, dn);
		} else if (is_in_byte_delimiters(*tmp_ptr, byte_delimiters_to_exclude, byte_delimiters_to_exclude_size)) {
			if (strlen(buff) > 0) {
				token* t = create_token(buff, strlen(buff));	
				token_list_node* tn = create_token_list_node(t); 
				push_token_list_node(&head, &tail, tn);
				bzero(buff, sizeof buff);
			}
		} else {
			int len = strlen(buff);
			buff[len] = (*tmp_ptr);
		}
		++tmp_ptr;
	}

	free(padded_s);
	return head;
}

bool delete_token_list(token_list_node* head) {
	if (NULL == head) return true;
	token_list_node* it = head;
	while (it) {
		token_list_node* next = delete_token_list_node(it);  		
		it = next;
	}	
	return true;
}

int main() {
	char s[1024] = "55 +            68   *                          (                       5 *     72              + 32 * 3 *1*1 *2* 1*1*                          (4              +    5              *       (                                       7+3+2+3         *4*(            5+2*3*2                         )*5*5 ) )                         )";
	char byte_delimiters_to_include[1024] = "+-*/()";
	char byte_delimiters_to_exclude[1024] = " \t";

	token_list_node* head = create_token_list(s, strlen(s), byte_delimiters_to_include, strlen(byte_delimiters_to_include), byte_delimiters_to_exclude, strlen(byte_delimiters_to_exclude));

	print_token_list_delimited_by_space(head);

	delete_token_list(head);
	return 0;
}
