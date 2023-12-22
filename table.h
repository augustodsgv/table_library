#ifndef TABLE_H
#define TABLE_H

typedef struct{
    int header;
    int line_division;
    int column_division;
    char * data_type;      // Define o tipo de atributos que estarão na tabela
}table_config;

typedef struct{
    char ** labels;
    int labels_count;
}labels;

typedef struct{
    void *** matrix;
    int n_lines;
    int n_columns;
    char * type;
}matrix;

// void print_table(matrix mat, table_config config, labels * table_labels);
void print_table(void *** matrix, int n_lines, int n_columns, labels * table_labels, table_config config);
void table_line_separator(char *** matrix, int n_lines, int n_columns, int * columns_length);
void simple_table(char *** matrix, int n_lines, int n_columns, int * columns_length);
void print_element(char * elemento, int column_length);
void header(labels * table_labels, int * column_length);
void separator_line(int n_columns, int * column_length);
int * get_columns_length(char *** matrix, labels * table_labels, int n_lines, int n_columns);
char *** convert_matrix(void ** matrix, int n_lines, int n_columns, const char * origin_type);



#endif /*TABLE_H*/