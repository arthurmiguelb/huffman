#include "../inc/decompress.h"

int is_bit_i_set(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

long long int get_filesize(char filePath[])
{
    FILE *file;
    file = fopen(filePath, "r");

    fseek(file, 0, SEEK_END);
    return ftell(file);              
}

huff_node *mount_tree(FILE *fileIn, short int *treesize)
{
    if(*treesize > 0)
    {         
        int flag = 0;    
        *treesize = *treesize - 1;

        unsigned char caractere = getc(fileIn);

        if(caractere == '\\')
        {
            caractere = getc(fileIn);
            *treesize = *treesize - 1;
            flag = 1;            
        }                      
        
        huff_node *root = create_huffman_node(set_void_pointer(caractere), 0);

        if(caractere == '*' && !flag)
        {                             
            place_tree_node(root, mount_tree(fileIn, treesize), root->right);                     
            place_tree_node(root, root->left,mount_tree(fileIn, treesize));
        }
        
        return root;
    }
}

void fill_file(FILE *fileIn, FILE *fileOut, huff_node *tree, short int trashsize, long long int file_size)
{    
    int i, j, lim = 0;
    unsigned char caractere;  
    huff_node *current = tree;  
    
    for(j = 0; j < file_size; j++)
    {
        caractere = getc(fileIn);

        if(j == (file_size - 1)) lim = trashsize;

        for(i = 7; i >= lim; i--)
        {
            if(is_bit_i_set(caractere, i))
            {
                current = current->right;
            }
            else
            {
                current = current->left;
            }
            
            if(is_leaf(current))
            {
                fprintf(fileOut, "%c", get_item(current));
                current = tree;                
            }            
        }                
    }
}

void decompress(char fileInPath[], char fileOutPath[])
{   
    FILE *fileIn, *fileOut;
    fileIn = fopen(fileInPath, "r");
    fileOut = fopen(fileOutPath, "w");

    huff_node *tree;
    
    long long int file_size = get_filesize(fileInPath);      

    short int trashsize = 0, treesize = 0;
    unsigned char buffer = getc(fileIn);
    
    trashsize = buffer >> 5;    
    buffer <<= 3;    
    buffer >>= 3;    
    treesize = buffer;
    treesize <<= 8;
    buffer = getc(fileIn);
    treesize = treesize | buffer;

    file_size -= (treesize + 2);
    
    tree = mount_tree(fileIn, &treesize);

    fill_file(fileIn, fileOut, tree, trashsize, file_size);    

    fclose(fileIn);
    fclose(fileOut); 
}
