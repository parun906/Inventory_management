#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// This file makes inputing strings easier
#include "helpers.c"

#define MAX_WORD 45
#define MAX_LINE 45 * 5
#define TABLESIZE 1000
#define MAX_LINE_SIZE 200
#define MAX_ITEM 200
#define MAX_SIZE 100
#define MAX_NAME_SIZE 50

//inventory
// Dynamic list of keys to store all the keys in inventory
int *keys = NULL;
int numofkeys = 0;

int no_of_customers = 0;

//pointer to reference memory as customer.name to be freed later
char *ptr_to_free[MAX_ITEM];
static int ptr_to_free_index = 0;

//flag for restock
bool restock_alarm = false;

int *restock_list = NULL;
static int restock_count = 0;

// Typedef for item
typedef struct
{
    int key;
    char *name;
    int threshold;
    int stock;
    float price;
} item;

// Typedef for nodes of hashtable
typedef struct node
{
    item items;
    struct node *next;
} node;

// HashTable declaration
node *Table[TABLESIZE];

//checkout
typedef struct gItem
{
    int key;
    int amount;
} gItem;

//customer checkout detail
typedef struct customer_detail
{
    char *name;
    float cash;
    gItem grocery_list[MAX_SIZE];
    int no_of_items;
} customer_detail;

// queue implementation
typedef struct queue
{
    customer_detail data;
    struct queue *next;
} queue;

queue *rear, *front;

//inventory functions
//hash table functions
int hash(int key);
void load();
bool search(int key);
item query(int key);
void unload();
void update_txt();

//inventory management functions
bool add(int key, char *name, int threshold, int stock, float price);
bool delete_item(int key);
bool restock(int key, int num);
bool restock_all();

//checkut functions

void enqueue(customer_detail new_data);
customer_detail dequeue(void);

int checkout(char *);
//make a receipt file
void make_receipt(char *filename);

//write receipt for each customer
void write_receipt(customer_detail customer, FILE *fptr);

//print inventory warning for restocking
void print_warning();

// helper functions
long extract_int(char *);

//get struct variable storing customer details
customer_detail make_customer(char[], float, int[], int);

//free pointer used to memory allocation of customer.name
void free_name_pointer_in_cd_struct();

//check id item is already in restock_list
bool is_in_restock_list(int key);

//main functions
void main_menu();
void inventory_submenu();

void when_exit();

int main(void)
{
    load();
    printf("--------------Inventory Management System--------------\n\n");
    main_menu();
    when_exit();
    }

void main_menu()
{
    printf("\n-------Main Menu-------\n");
    printf("CHOOSE YOUR OPTION\n");
    printf("> checkout test.txt\n");
    printf("> inventory\n");
    printf("> exit\n");
    printf("\n>Enter your choice with valid syntax:\n");
    string input = get_string("> ");
    if (strlen(input) != 0)
    {
        string option = strtok(input, " ");
        if (strcmp("checkout", option) == 0)
        {
            string filename = strtok(NULL, " ");
            string trail = strtok(NULL, " ");
            if (filename == NULL || trail != NULL)
            {
                printf("> Invalid syntax\n");
                main_menu();
            }
            checkout(filename);
            printf("Message --Checkout done--\n");
            main_menu();
        }

        else if (strcmp("inventory", option) == 0)
        {
            string trail = strtok(NULL, " ");
            if (trail != NULL)
            {
                printf("> Invalid Syntax\n");
                main_menu();
            }
            inventory_submenu();
            main_menu();
        }

        else if (strcmp("exit", option) == 0)
        {
            printf("----- Exiting-----\n");
            when_exit();
            exit(0);
        }

        else
        {
            printf("> No such option\n");
            main_menu();
        }
    }
    else
    {
        printf("> Invalid Syntax\n");
        main_menu();
    }
}

void inventory_submenu()
{
    printf("\n\n-------Inventory Submenu-------\n");
    printf("CHOOSE YOUR OPTION\n");
    printf("> add item (syntax: add key name threshold stock price)\n");
    printf("> delete item (syntax: delete key)\n");
    printf("> restock item (syntax: restock key num)\n");
    printf("> restock all (syntax: restock all\n");
    printf("> return to main menu (syntax return)\n");
    printf("\n>Enter your choice with valid syntax:\n");
    string input = get_string("> ");
    if (strlen(input) != 0)
    {
        string option = strtok(input, " ");
        if (strcmp("add", option) == 0)
        {
            string key = strtok(NULL, " ");
            string name = strtok(NULL, "\"");
            string threshold = strtok(NULL, " ");
            string stock = strtok(NULL, " ");
            string price = strtok(NULL, " ");
            string trail = strtok(NULL, " ");
            if (trail != NULL || key == NULL || name == NULL || threshold == NULL || stock == NULL || price == NULL)
            {
                printf("> Invalid Syntax (Check the data types and if you didn't enclose string with quotation)");
                inventory_submenu();
            }

            if (!is_int(key) || !is_int(threshold) || !is_int(stock) || !is_float(price))
            {
                    printf("> Invalid syntax (Check the data types and if you didn't enclose string with quotation)\n");
                    inventory_submenu();
            }

            if (!add(atoi(key), name, atoi(threshold), atoi(stock), atof(price)))
            {
                printf("> Error --Data cannot be added--!!\n");
            }
            else
            {
                printf("Message --Added item with key %s--\n", key);
                inventory_submenu();
            }
        }

        else if (strcmp("delete", option) == 0)
        {
            string key = strtok(NULL, " ");
            string trail = strtok(NULL, " ");
            if (trail != NULL || key == NULL)
            {
                printf("> Invalid Syntax\n");
                inventory_submenu();
            }

            if (!is_int(key))
            {
                printf("Error --Key should be an integer--\n");
                inventory_submenu();
            }
            if (!delete_item(atoi(key)))
            {
                printf("> Error --Could not delete item--\n");
            }
            else
            {
                printf("Message --Deleted item %s key--\n", key);
                inventory_submenu();
            }
        }

        else if (strcmp("restock", option) == 0)
        {
            string key = strtok(NULL, " ");
            string num = strtok(NULL, " ");
            string trail = strtok(NULL, " ");
            if (key == NULL)
            {
                printf("> Invalid syntax\n");
                inventory_submenu();
            }

            if (strcmp("all", key) == 0 && num == NULL)
            {
                if(!restock_all())
                {
                    printf("Error --Could not restock item--\n");
                }
                else
                {
                    printf("Message --Restocked all succesfully--\n");
                    inventory_submenu();
                }
            }

            else if (num != NULL && trail == NULL)
            {
                if (!is_int(num) && !is_int(key))
                {
                    printf("Error --Num and key should be an integer--\n");
                    inventory_submenu();
                }
                if(!restock(atoi(key), atoi(num)))
                {
                    printf("Error --Item with key %s could not be restocked--\n", key);
                }
                else
                {
                    printf("Message --Restocked item %s succefully--\n", key);
                    inventory_submenu();
                }
            }

            else
            {
                printf("> Invalid syntax\n");
                inventory_submenu();
            }
        }

        else if (strcmp("return", option) == 0)
        {
            main_menu();
        }

        else
        {
            printf("> Invalid Syntax!\n");
            inventory_submenu();
        }
    }
    else
    {
        printf("> Invalid syntax\n");
        inventory_submenu();
    }
}

//final build needs to have a better hash function this woks for now
int hash(int key)
{
    const float A = 0.61803398875;
    int hash_key = floor(TABLESIZE * (key * A - floor(key * A)));
    return hash_key;
}

// Function to load the hash table
void load()
{
    // Opening file
    FILE *file = fopen("inventory.txt", "r");
    if (file == NULL)
    {
        printf("Error! --Inventory.txt could not be opened--\n");
        when_exit();
        exit(1);
    }

    // Reading from file then parsing the read data
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] != '\n')
        {
            int i = 0;
            int j = 0;
            int k = 0;
            char words[5][MAX_WORD];
            while (line[i] != '\0')
            {
                if (line[i] == ',')
                {
                    words[j][k] = '\0';
                    j++;
                    i = i + 2;
                    k = 0;
                }

                else if (line[i] == '"' || line[i] == '{')
                {
                    i++;
                }

                else if (line[i] == '}')
                {
                    words[j][k] = '\0';
                    break;
                }
                else
                {
                    words[j][k] = line[i];
                    k++;
                    i++;
                }
            }
            if (!add(atoi(words[0]), words[1], atoi(words[2]), atoi(words[3]), atof(words[4])))
            {
                printf("Error --Loading failed--\n");
                when_exit();
                exit(-1);
            }
        }
    }
    fclose(file);
    return;
}

bool search(int key)
{
    int hash_key = hash(key);
    node *temp;
    temp = Table[hash_key];
    while (temp != NULL)
    {
        if (temp->items.key == key)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// Returns the item with the given key
item query(int key)
{
    int hash_key = hash(key);
    node *n = Table[hash_key];
    while (n != NULL)
    {
        if (n->items.key == key)
        {
            return n->items;
        }
        n = n->next;
    }
    item item_X = {.key = -122, .name = "Not Found", .threshold = -122, .stock = -120, .price = 0};
    return item_X;
}
// Updates the text file at the end of the program
void update_txt()
{
    FILE *file = fopen("inventory.txt", "w");
    for (int i = 0; i < numofkeys; i++)
    {
        int hash_key = hash(keys[i]);
        node *n = Table[hash_key];
        while (n != NULL)
        {
            fprintf(file, "{%.3i, \"%s\", %i, %i, %.2f}\n", n->items.key, n->items.name, n->items.threshold, n->items.stock, n->items.price);
            n = n->next;
        }
    }
    fclose(file);
}

// Adds into hash table
bool add(int key, string name, int threshold, int stock, float price)
{
    if (search(key))
    {
        printf("> Warning: Key already exists so item cannot be added\n");
        return 0;
    }

    // Data assigned to item;
    item item_X;
    item_X.key = key;
    item_X.name = malloc(sizeof(char) * (strlen(name) + 1));
    if (item_X.name == NULL)
    {
        printf("ERROR! -----Memory Allocation Failed-----!!\n");
        when_exit();
        exit(-1);
    }
    strcpy(item_X.name, name);
    item_X.threshold = threshold;
    item_X.stock = stock;
    item_X.price = price;

    // Adding item to hash table
    int hash_key = hash(item_X.key);
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        printf("ERROR! -----Memory Allocation Failed-----!!\n");
        when_exit();
        exit(1);
    }
    n->items = item_X;
    n->next = Table[hash_key];
    Table[hash_key] = n;

    // Add keys in list
    int *temp = realloc(keys, sizeof(int) * (numofkeys + 1));
    keys = temp;
    keys[numofkeys] = item_X.key;
    numofkeys++;
    return 1;
}

// Deletes item from hash table
bool delete_item(int key)
{
    if (search(key))
    {
        int hash_key = hash(key);
        node *temp = Table[hash_key];
        while (temp != NULL)
        {
            if (temp->items.key == key)
            {
                free(temp->items.name);
                node *temp1 = temp->next;
                while (temp1 != NULL)
                {
                    if (temp1->next == NULL)
                    {
                        temp1->next = Table[hash_key];
                        break;
                    }
                    temp1 = temp1->next;
                }
                Table[hash_key] = temp->next;
                free(temp);
                break;
            }
            temp = temp->next;
        }
        return 1;
    }
    return 0;
}

// Function to restock the 'key' item by adding 'num' amount
bool restock(int key, int num)
{
    if (search(key))
    {
        int hash_key = hash(key);
        node *temp = Table[hash_key];
        while (temp != NULL)
        {
            if (temp->items.key == key)
            {
                temp->items.stock += num;
                return 1;
            }
            temp = temp->next;
        }
        return 1;
    }
    return 0;
}

// Restocks all the items to threshold value
bool restock_all()
{
    for (int i = 0; i < numofkeys; i++)
    {
        int hash_key = hash(keys[i]);
        node *n = Table[hash_key];

        while (n != NULL)
        {
            if (n->items.key == keys[i])
            {
                if (n->items.stock < n->items.threshold)
                {
                    n->items.stock = n->items.threshold;
                }
            }
            n = n->next;
        }
    }
    return 1;
}

// Function frees all the memories
void unload()
{
    for (int i = 0; i < numofkeys; i++)
    {
        if (search(keys[i]))
        {
            delete_item(keys[i]);
        }
    }
    free(keys);
}

int checkout(char *filename)
{
    rear = NULL, front = NULL;
    no_of_customers = 0, ptr_to_free_index = 0;

    FILE *fptr = NULL;
    restock_list = (int *)malloc(sizeof(int));

    //open file
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("Error --file could not be opened--\n");
        when_exit();
        exit(1);
    }

    char *line;

    line = (char *)calloc(MAX_LINE_SIZE, sizeof(char));
    if (line == NULL)
    {
        printf("Error --Memory allocation for line failed--\n");
        when_exit();
        exit(1);
    }

    int j;
    char *token, *str, name[MAX_NAME_SIZE];
    float cash;
    long num;

    //parse register file and enqueue customer
    while (fgets(line, MAX_LINE_SIZE, fptr))
    {
        int glist[MAX_ITEM], gindex = 0;
        for (j = 1, str = line;; j++, str = NULL)
        {
            /* line format eg: {"Karen", 8.00, [{102, 3}, {216, 1}]}
             * separate substrings with "," as delimeter
             */
            token = strtok(str, ",");

            //if no further substring break for loop
            if (token == NULL)
                break;

            //first extract name
            if (j == 1)
            {
                int index = 0;
                while (*token != '\0')
                {
                    if (isalpha(*token) || *token == ' ')
                        name[index++] = *token;
                    token++;
                }
                name[index] = '\0';
            }
            //second extract cash
            else if (j == 2)
            {
                cash = atof(token);
            }
            //extract numbers and place in 1D array for grocey list
            else
            {
                num = extract_int(token);
                glist[gindex++] = num;
            }
        }

        // get a customer with details
        customer_detail customer = make_customer(name, cash, glist, gindex / 2);

        // enqueue customer
        enqueue(customer);

        no_of_customers++;
    }

    free(line);
    free(str);

    //make receipt file
    make_receipt(filename);

    //print the inventory warning if necessary
    print_warning();

    free(restock_list);
    fclose(fptr);
    return 0;
}

//extract integer from mixed string
long extract_int(char *ptr)
{
    long val = 0;
    char *endptr;
    while (*ptr != '\0' || *ptr)
    {
        // print_c(*ptr);
        if (isdigit(*ptr))
        {
            val = strtol(ptr, &endptr, 10);
            // printf("%ld\n", val);
            return val;
        }
        else
        {
            ptr++;
        }
    }
    return val;
}

// make variable of customer_detail struct
customer_detail make_customer(char name[], float cash, int grocery_list[], int nO_of_items_in_g_list)
{
    customer_detail customer;

    // allocate memory for customer.name
    customer.name = malloc((strlen(name) + 1) * sizeof(char));
    if (customer.name == NULL)
    {
        printf("Error!! --Memory allocation failed!--\n");
        when_exit();
        exit(1);
    }

    //copy value of name to memory allocated
    strcpy(customer.name, name);

    //make a pointer to reference to the allocated memory
    //so that at the end of the program we could free it
    ptr_to_free[ptr_to_free_index++] = customer.name;

    customer.cash = cash;

    for (int i = 0, k = 0; i < nO_of_items_in_g_list; i++, k += 2)
    {
        customer.grocery_list[i].key = grocery_list[k];
        customer.grocery_list[i].amount = grocery_list[k + 1];
    }

    customer.no_of_items = nO_of_items_in_g_list;

    return customer;
}

void make_receipt(char *filename)
{
    // printf("%s", filename);
    customer_detail customer;

    //generate name for receipt file
    char *filename_cpy, *basename, *new_name;
    filename_cpy = malloc((strlen(filename) + 1) * (sizeof(char)));
    strcpy(filename_cpy, filename);
    basename = strtok(filename_cpy, ".");
    basename = realloc(basename, (strlen(basename) + 13) * sizeof(char)); //13 = length of _receipt.txt
    strcat(basename, "_receipt.txt");

    FILE *fptr;

    // open file for writing
    fptr = fopen(basename, "w");
    if (fptr == NULL)
    {
        printf("Error! --File could not be opened--\n");
        when_exit();
        exit(1);
    }

    for (int i = 0; i < no_of_customers; i++)
    {
        //dequeue customer from queue
        customer = dequeue();

        //write receipt for dequeued customer
        write_receipt(customer, fptr);
    }

    fclose(fptr);
    free(basename);
    basename = NULL;

    return;
}

void write_receipt(customer_detail customer, FILE *fptr)
{

    float total = 0, item_total = 0;
    int change_by;

    fprintf(fptr, "Customer - %s\n\n", customer.name);

    item itemDetail;

    // make pointer to the grocery_list array
    gItem *gList, *beginning;
    beginning = customer.grocery_list;
    gList = beginning;

    //go through grocery list one by one
    for (int i = 0; i < customer.no_of_items; i++)
    {
        //store item details(key, name, threshold, price etc) on itemDetails
        if(!search(gList->key))
            continue;
        itemDetail = query(gList->key);

        //decrease the item in stock by amount
        change_by = -1 * gList->amount;
        restock(gList->key, change_by);

        //calculate total amount for the item
        item_total = gList->amount * itemDetail.price;

        fprintf(fptr, "%s X%d @ $%.2f\n", itemDetail.name, gList->amount, itemDetail.price);

        total += item_total;
        gList++;
    }

    fprintf(fptr, "\nTotal: $%.2f\n\n", total);

    //check if customer has enough cash, if not add items back to stock
    if (total <= customer.cash)
    {
        fprintf(fptr, "Thank you, come back soon!\n");
    }
    else
    {
        gList = beginning;
        for (int i = 0; i < customer.no_of_items; i++)
        {
            change_by = gList->amount;
            restock(gList->key, change_by);
            gList++;
        }
        fprintf(fptr, "Customer did not have enough money and was REJECTED\nThank you, come back soon!\n");
    }

    fprintf(fptr, "-------------------------------------------------------\n\n");

    //find items needing restocking
    gList = beginning;
    for (int i = 0; i < customer.no_of_items; i++)
    {
        itemDetail = query(gList->key);
        if ((itemDetail.stock < itemDetail.threshold) && !is_in_restock_list(gList->key))
        {
            restock_count++;
            size_t size = sizeof(int) * (restock_count + 1);

            int *temp = realloc(restock_list, size);
            if (temp == NULL)
            {
                printf("Error!! --Memory cannot be re reallocated--\n");
                when_exit();
                exit(1);
            }

            restock_list = temp;
            restock_list[restock_count - 1] = gList->key;

            restock_alarm = true;
        }
        gList++;
    }
}

void print_warning()
{
    int restock_item, count;
    item item_detail;

    if (restock_count)
    {
        printf("\nWarning! The following Item(s) may need to be restocked:\n");
    }

    for (int k = 0; k < restock_count; k++)
    {
        restock_item = restock_list[k];
        if (!search(restock_item))
            continue;
        item_detail = query(restock_item);
        printf("%d (%s): %d remain in stock, replenishment threshold is %d\n", item_detail.key, item_detail.name, item_detail.stock, item_detail.threshold);
    }
}

//find id item is already in restock_list
bool is_in_restock_list(int key)
{
    for (int i = 0; i < restock_count; i++)
    {
        if (restock_list[i] == key)
        {
            return true;
        }
    }
    return false;
}

// function to free all the allocated memory to customer.name
void free_name_pointer_in_cd_struct()
{

    for (int i = 0; i < no_of_customers; i++)
    {
        free(ptr_to_free[i]);
        ptr_to_free[i] = NULL;
    }
}

// enqueue customer detail
void enqueue(customer_detail new_data)
{
    queue *new_node;

    new_node = (queue *)malloc(sizeof(queue));
    if (new_node == NULL)
    {
        printf("Error! --Memory Allocation failed--\n");
        when_exit();
        exit(1);
    }

    if (rear == NULL)
    {
        new_node->data = new_data;
        new_node->next = NULL;
        front = rear = new_node;
    }
    else
    {
        new_node->data = new_data;
        new_node->next = NULL;
        rear->next = new_node;
        rear = new_node;
    }
}

// dequeue customer detail
customer_detail dequeue()
{
    queue *temp;
    customer_detail customer_info;

    if (front == NULL)
    {
        printf("Queue is empty! \n");
        when_exit();
        exit(1);
    }
    else if (front->next == NULL)
    {
        temp = front;
        customer_info = temp->data;
        front = rear = NULL;
        free(temp);
    }
    else
    {
        temp = front;
        customer_info = temp->data;
        front = front->next;
        free(temp);
    }

    return customer_info;
}

void when_exit()
{
    update_txt();
    atend();
    unload();
    free_name_pointer_in_cd_struct();
}
