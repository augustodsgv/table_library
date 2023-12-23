#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "table.h"

#define string_size 5
void *** matrix_cached;
int n_lines_cached;
int n_columns_cached;
labels * table_labels_cached;
table_config * config_cached;
int cache_avaible = 0;

// Printa novamente a úlima matrix. Como recebe ponteiros, ajuda a não precisar de 
int print_cached(){
    if(cache_avaible){
        print_table(matrix_cached, n_lines_cached, n_columns_cached, table_labels_cached, config_cached);
        return 1;
    }
    return 0;
}

void fill_cache(void *** matrix, int n_lines, int n_columns, labels * table_labels, table_config * config){
    matrix_cached = matrix;
    n_lines_cached = n_lines;
    n_columns_cached = n_columns;
    table_labels_cached = table_labels;
    config_cached = config;
    cache_avaible = 1;
}

// Print tables in a formatable fortmat
void print_table(void *** matrix, int n_lines, int n_columns, labels * table_labels, table_config * config){
    fill_cache(matrix, n_lines, n_columns, table_labels, config);
    char *** string_matrix;
    if(!strcmp(config->data_type, "%s") || !strcmp(config->data_type, "%c"))
        string_matrix = (char***)matrix;
    else
        string_matrix = convert_matrix(*matrix, n_lines, n_columns, config->data_type);
    
    int * columns_length;
    if (config->header){
        columns_length = get_columns_length(string_matrix, table_labels, n_lines, n_columns);
        if (table_labels == NULL){
            // TODO : Verificar como usar o stderr para exibir essa mensagem
            printf("From: table library -> ");
            printf("Error: trying to create header but NULL label srray was provided\n");
            exit(1);
        }
        header(table_labels, columns_length);
    }else
        columns_length = get_columns_length(string_matrix, NULL, n_lines, n_columns);
    if(config->line_division){
        table_line_separator(string_matrix, n_lines, n_columns, columns_length);
        return;
    }
    simple_table(string_matrix, n_lines, n_columns, columns_length);
    // TODO : Adicionar novos tipos de tabela

}

// Printa a tabela, somente
void table_line_separator(char *** matrix, int n_lines, int n_columns, int * columns_length){
    if(columns_length == NULL)
        columns_length = get_columns_length(matrix, NULL, n_lines, n_columns);
    
    for(int i = 0; i < n_lines; i++){
        separator_line(n_columns, columns_length);
        for(int j = 0; j < n_columns; j++){
            print_element(matrix[i][j], columns_length[j]);
        }
        printf("|\n");
    }
    separator_line(n_columns, columns_length);
}

// Printa a tabela, somente
void simple_table(char *** matrix, int n_lines, int n_columns, int * columns_length){
    if(columns_length == NULL)
        columns_length = get_columns_length(matrix, NULL, n_lines, n_columns);
    separator_line(n_columns, columns_length);
    
    for(int i = 0; i < n_lines; i++){
        for(int j = 0; j < n_columns; j++){
            print_element(matrix[i][j], columns_length[j]);
        }
        printf("|\n");
    }
    separator_line(n_columns, columns_length);
}

// Printa o elemento da matriz de maneira a obedecer o tamanho da coluna
void print_element(char * elemento, int column_length){
    int diff = column_length - strlen(elemento);
    printf("|");
    for(int i = 0; i <= (diff + 1) / 2 ; i++) printf(" ");       // Vamos deixar como padrão ele alinhar a direita, no caso de tamanhos ímpares
    printf("%s", elemento);
    for(int i = 0; i <= diff / 2; i++) printf(" ");
}

// Função que printa os headers e retorna um vetor com o tamanho que as colunas devem ter
void header(labels * table_labels, int * column_length){
    separator_line(table_labels->labels_count, column_length);
    for(int i = 0; i < table_labels->labels_count; i++) printf("| %s ", table_labels->labels[i]);
    printf("|\n");
}

// Função que printa uma linha de separação
void separator_line(int n_columns, int * column_length){
    for (int i = 0; i < n_columns; i++){
        printf("+");
        for(int j = 0; j < column_length[i] + 2; j++) printf("-");      // Printa 2 espaços a mais para os lados
    }
    printf("+\n");
}

// Retorna o tamanho ideal que cada coluna deve ter de maneira que todos elementos caibam nas colunas
// Passar NULL para tabelas sem Label
int * get_columns_length(char *** matrix, labels * table_labels, int n_lines, int n_columns){
    // Alocando o vetor de tamanhos
    int * columns_length;
    if (table_labels != NULL && n_columns != table_labels->labels_count){
        // TODO : Verificar como usar o stderr para exibir essa mensagem
        printf("From: table library -> ");
        printf("Warning: different number of labels and columns columns\n");
        columns_length = calloc(n_columns, sizeof(int) * (n_columns > table_labels->labels_count    // Using calloc so it begins with 0
                                                                    ? n_columns                     // Picking the longest between both
                                                                    : table_labels->labels_count));        
    }else{
        columns_length = calloc(n_columns, sizeof(int) * n_columns);
    }

    if(table_labels != NULL){
        for(int i = 0; i < table_labels->labels_count; i++)             // Calculando os tamanhos dos labels
            if (strlen(table_labels->labels[i]) > columns_length[i])
                columns_length[i] = strlen(table_labels->labels[i]);
    }

    for(int i = 0; i < n_lines; i++)                                // Calculando os tamanhos dos elementos
        for(int j = 0; j < n_columns; j++)
            if (strlen(matrix[i][j]) > columns_length[j]){
                NULL;
                columns_length[j] = strlen(matrix[i][j]);
            }
    return columns_length;
}

// Função que converte a matriz para string para que as demais funções funcionem adequadamente
char *** convert_matrix(void ** matrix, int n_lines, int n_columns, const char * origin_type){
    // Alocando a matrix de strings. É necessário um vetor de 3 dimensões pois a string é um vetor unidimensional
    char *** char_matrix = malloc(sizeof(char**) * n_lines);
    for (int i = 0; i < n_lines; i++){
        char_matrix[i] = malloc(sizeof(char*) * n_columns);
        for(int j = 0; j < n_columns; j++){
            char_matrix[i][j] = malloc(sizeof(char) * string_size);
            char type = origin_type[1];
            switch (type){
            // TODO : adicionar novos tipos (são úteis?)
            case 'd':
                sprintf(char_matrix[i][j], origin_type, ((int**)matrix)[i][j]);      // Conversão inteiro
                break;
            case 'f':
                sprintf(char_matrix[i][j], origin_type, ((float**)matrix)[i][j]);      // Fazendo a conversão
                break;
            default:
                // TODO : Verificar como usar o stderr para exibir essa mensagem
                printf("From: table library -> ");
                printf("Error: trying to convert from unsupported type\n");
                // TODO : Verificar como deesalocar a memória da melhor forma
                exit(1);
            }
                
        }
    }
    return char_matrix;
}

void print_matrix(void *** matrix, int n_lines, int n_columns, const char * origin_type){
    for (int i = 0; i < n_lines; i++){
        for(int j = 0; j < n_columns; j++){
            char type = origin_type[1];
            switch (type){
            case 's':
                for (int i = 0; i < n_lines; i++){
                    for(int j = 0; j < n_columns; j++)
                        printf("%s ", matrix[i][j]);
                    printf("\n");
                }
                break;
            case 'c':
                char ** matrix_print = *(char***)matrix;
                for (int i = 0; i < n_lines; i++){
                    for(int j = 0; j < n_columns; j++)
                        printf("%c ", matrix_print[i][j]);
                    printf("\n");
                }
                break;
            case 'd':
                int ** matrix_print = *(int***)matrix;
                for (int i = 0; i < n_lines; i++){
                    for(int j = 0; j < n_columns; j++)
                        printf("%d ", matrix_print[i][j]);
                    printf("\n");
                }
                break;
            case 'f':
                float ** matrix_print = *(float***)matrix;
                for (int i = 0; i < n_lines; i++){
                    for(int j = 0; j < n_columns; j++)
                        printf("%f ", matrix_print[i][j]);
                    printf("\n");
                }
                break;
            default:
                // TODO : Verificar como usar o stderr para exibir essa mensagem
                printf("From: table library -> ");
                printf("Error: trying to print matrix from unsupported type\n");
                // TODO : Verificar como deesalocar a memória da melhor forma
                exit(1);
            }
                
        }
    }
}
