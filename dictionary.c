#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_RANGE 26
#define WORD_SIZE 50
#define DATAFILE "dsaproject1.txt"
#define MAX 256

#define CHAR_TO_INDEX(c) ((int)tolower(c) - (int)'a')
#define INDEX_TO_CHAR(i) (char)((int)'a' + i)

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_RANGE];
    bool isEnd;
} node;

node* createNode() {
    node* newNode = (node*)malloc(sizeof(node));
    newNode->isEnd = false;
    for (int i = 0; i < ALPHABET_RANGE; i++)
        newNode->children[i] = NULL;
    return newNode;
}

void insert(node* root, char* word) {
    if (*word == '\0') return;
    do {
        int i = CHAR_TO_INDEX(*word);
        if (root->children[i] == NULL)
            root->children[i] = createNode();
        root = root->children[i];
    } while (*(++word) != '\0');
    root->isEnd = true;
}

bool isLeaf(node* t) {
    for (int i = 0; i < ALPHABET_RANGE; i++) {
        if (t->children[i] != NULL)
            return false;
    }
    return true;
}

int is_leaf_node(node* root, char* word) {
    node* temp = root;
    for (int i = 0; word[i]; i++) {
        int position = (int)word[i] - 'a';
        if (temp->children[position]) {
            temp = temp->children[position];
        }
    }
    return temp->isEnd ? 1 : 0;
}

void deleteWord(node* root, char* word) {
    if (*word == '\0') return;
    do {
        int i = CHAR_TO_INDEX(*word);
        if (root->children[i] == NULL)
            return;
        root = root->children[i];
    } while (*(++word) != '\0');
    root->isEnd = false;
}

void printWord(char* str, int n) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%c ", str[i]);
    }
}

void printAllWords(node* root, char* wordArray, int pos) {
    if (root == NULL)
        return;

    if (root->isEnd) {
        printWord(wordArray, pos);
    }
    for (int i = 0; i < ALPHABET_RANGE; i++) {
        if (root->children[i] != NULL) {
            wordArray[pos] = i + 'a';
            printAllWords(root->children[i], wordArray, pos + 1);
        }
    }
}

void suggestions_recursive(node* root, char prefix[]) {
    if (root->isEnd)
        printf("%s\n", prefix);

    if (isLeaf(root)) return;

    for (int i = 0; i < ALPHABET_RANGE; i++) {
        if (root->children[i]) {
            char newPrefix[ALPHABET_RANGE];
            strcpy(newPrefix, prefix);
            strcat(newPrefix, (char[2]){INDEX_TO_CHAR(i), '\0'});
            suggestions_recursive(root->children[i], newPrefix);
        }
    }
}

int get_suggestions(node* trie, char query[]) {
    int n = strlen(query);
    for (int i = 0; i < n; i++) {
        int index = CHAR_TO_INDEX(query[i]);
        if (!trie->children[index]) return 0;
        else trie = trie->children[index];
    }

    bool isWord = trie->isEnd;
    bool isLast = isLeaf(trie);

    if (isWord && isLast) {
        printf("%s", query);
        return -1;
    }

    if (!isLast) {
        char prefix[100];
        strcpy(prefix, query);
        suggestions_recursive(trie, prefix);
        return 1;
    }

    return 0;
}

bool wordExists(node* root, char* word) {
    for (int i = 0; word[i]; i++) {
        int position = (int)word[i] - 'a';
        if (root->children[position]) {
            root = root->children[position];
        } else {
            return false;
        }
    }
    return root->isEnd;
}

void freeTrie(node* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_RANGE; i++) {
        freeTrie(root->children[i]);
    }
    free(root);
}

void setup_trie(node** trie, char filename[]) {
    FILE* fp = fopen(filename, "r");
    while (!feof(fp)) {
        char word[100];
        fscanf(fp, "%s", word);
        insert(*trie, word);
    }
    fclose(fp);
}

int main() {
    node* trie = createNode();
    int n;
    char input[WORD_SIZE] = "";
    char q[WORD_SIZE] = "";
    char s[WORD_SIZE] = "";

    setup_trie(&trie, DATAFILE);

    do {
        printf("\n\n*\n");
        printf("Menu:\n     1. Enter new word\n     2. Continue word\n     3. Add word to dictionary\n     4. Delete word from dictionary\n     5. Exit");
        printf("\n\n*\n");
        printf("\tEnter choice: ");
        scanf("%d", &n);

        switch (n) {
        case 1:
            printf("\n\n=================\n");
            printf("Enter prompt: ");
            scanf("%s", input);
            printf("======================\n\n");
            strcpy(q, input);
            get_suggestions(trie, input);
            break;

        case 2:
            printf("\n\n=================\n");
            printf("Enter prompt: %s", q);
            scanf("%s", input);
            printf("======================\n\n");
            strcat(q, input);
            get_suggestions(trie, q);
            break;

        case 3:
            printf("\n\tEnter word to be added: ");
            scanf("%s", s);
            insert(trie, s);
            FILE* fp = fopen(DATAFILE, "a");
            fprintf(fp, "\n");
            fprintf(fp, s);
            fclose(fp);
            printf("\n\nThe word %s has been added.\n", s);
            break;

        case 4:
            printf("\n\tEnter word to be deleted: ");
            scanf("%s", s);

            if (!wordExists(trie, s)) {
                printf("\n\nThe word %s is not present in the trie.\n", s);
                break;
            }

            FILE* fileCheck = fopen(DATAFILE, "r");
            FILE* tempFile = fopen("temp.txt", "w");

            char fileWord[MAX];
            while (fscanf(fileCheck, "%s", fileWord) != EOF) {
                if (strcmp(s, fileWord) != 0) {
                    fprintf(tempFile, "%s\n", fileWord);
                }
            }

            fclose(fileCheck);
            fclose(tempFile);

            remove(DATAFILE);
            rename("temp.txt", DATAFILE);

            deleteWord(trie, s);

            printf("\n\nThe word %s has been deleted.\n", s);
            break;

        case 5:
            printf("\nExiting program");
            printf("\n*\n");
            break;
        }

    } while (n != 5);

    freeTrie(trie);

    return 0;
}
