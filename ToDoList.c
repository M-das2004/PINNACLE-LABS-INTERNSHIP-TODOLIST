#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_DESCRIPTION_LENGTH 256
#define INITIAL_CAPACITY 2

typedef struct {
    int id;
    char description[MAX_DESCRIPTION_LENGTH];
    bool completed;
} TodoItem;

TodoItem* todo_list = NULL;
int todo_count = 0;
int todo_capacity = 0;
int next_id = 1;

// Function Declarations
void initialize_todo_list();
void free_todo_list();
void ensure_capacity();
void add_todo(const char* description);
void list_todos();
void complete_todo(int id);
void edit_todo_description(int id, const char* new_description);
void delete_todo(int id);
void save_todos_to_file(const char* filename);
void load_todos_from_file(const char* filename);

// Initialize the todo list
void initialize_todo_list() {
    todo_capacity = INITIAL_CAPACITY;
    todo_list = (TodoItem*)malloc(sizeof(TodoItem) * todo_capacity);
    if (todo_list == NULL) {
        printf("Error: failed to allocate memory!\n");
        exit(EXIT_FAILURE);
    }
    todo_count = 0;
    next_id = 1;
}

// Free the todo list memory
void free_todo_list() {
    if (todo_list != NULL) {
        free(todo_list);
        todo_list = NULL;
    }
    todo_count = 0;
    todo_capacity = 0;
    next_id = 1;
}

// Ensure dynamic capacity
void ensure_capacity() {
    if (todo_count >= todo_capacity) {
        todo_capacity *= 2;
        TodoItem* temp = (TodoItem*)realloc(todo_list, sizeof(TodoItem) * todo_capacity);
        if (temp == NULL) {
            printf("Error: failed to reallocate memory!\n");
            exit(EXIT_FAILURE);
        }
        todo_list = temp;
    }
}

// Add a new task
void add_todo(const char* description) {
    if (strlen(description) == 0) {
        printf("Error: description cannot be empty.\n");
        return;
    }

    ensure_capacity();
    todo_list[todo_count].id = next_id++;
    strcpy(todo_list[todo_count].description, description);
    todo_list[todo_count].completed = false;
    todo_count++;

    printf("Success: TodoItem added (ID %d - \"%s\")\n",
        todo_list[todo_count - 1].id, todo_list[todo_count - 1].description);
}

// List all tasks
void list_todos() {
    if (todo_count == 0) {
        printf("The todo list is empty.\n");
        return;
    }

    printf("\n--- Current Todo Items (%d items) ---\n", todo_count);
    for (int i = 0; i < todo_count; i++) {
        printf("ID: %-4d | Status: %-8s | Description: %s\n",
            todo_list[i].id,
            todo_list[i].completed ? "COMPLETE" : "PENDING",
            todo_list[i].description);
    }
    printf("--------------------------------------\n");
}

// Mark a task as complete
void complete_todo(int id) {
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            if (todo_list[i].completed) {
                printf("Info: Todo ID %d is already complete.\n", id);
            } else {
                todo_list[i].completed = true;
                printf("Success: Todo ID %d marked as COMPLETE.\n", id);
            }
            return;
        }
    }
    printf("Error: Todo with ID %d not found.\n", id);
}

// ✏ Edit an existing task’s description
void edit_todo_description(int id, const char* new_description) {
    if (strlen(new_description) == 0) {
        printf("Error: new description cannot be empty.\n");
        return;
    }

    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            strcpy(todo_list[i].description, new_description);
            printf("Success: Todo ID %d updated to \"%s\".\n", id, new_description);
            return;
        }
    }
    printf("Error: Todo with ID %d not found.\n", id);
}

// Delete a task
void delete_todo(int id) {
    int found_index = -1;
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        for (int i = found_index; i < todo_count - 1; i++) {
            todo_list[i] = todo_list[i + 1];
        }
        todo_count--;
        printf("Success: Todo with ID %d deleted.\n", id);
    } else {
        printf("Error: Todo with ID %d not found.\n", id);
    }
}

// Save all tasks to file
void save_todos_to_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error: Could not open file for writing");
        return;
    }

    for (int i = 0; i < todo_count; i++) {
        fprintf(file, "%d;%d;%s\n",
            todo_list[i].id,
            todo_list[i].completed ? 1 : 0,
            todo_list[i].description);
    }
    fclose(file);
    printf("Todos saved to '%s'.\n", filename);
}

// Load tasks from file
void load_todos_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Info: No existing '%s' file found. Starting with an empty list.\n", filename);
        return;
    }

    free_todo_list();
    initialize_todo_list();

    char line[MAX_DESCRIPTION_LENGTH + 50];
    int loaded_id, loaded_completed;
    char loaded_description[MAX_DESCRIPTION_LENGTH];
    int max_id_found = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char* first_semi = strchr(line, ';');
        if (first_semi == NULL) continue;
        *first_semi = '\0';
        loaded_id = atoi(line);

        char* second_semi = strchr(first_semi + 1, ';');
        if (second_semi == NULL) continue;
        *second_semi = '\0';
        loaded_completed = atoi(first_semi + 1);

        strncpy(loaded_description, second_semi + 1, sizeof(loaded_description) - 1);
        loaded_description[strcspn(loaded_description, "\n")] = 0;

        ensure_capacity();
        todo_list[todo_count].id = loaded_id;
        strcpy(todo_list[todo_count].description, loaded_description);
        todo_list[todo_count].completed = (loaded_completed == 1);
        todo_count++;

        if (loaded_id >= max_id_found) {
            max_id_found = loaded_id;
        }
    }
    next_id = max_id_found + 1;
    fclose(file);
    printf("Info: Loaded %d todos from '%s'.\n", todo_count, filename);
}

// Main Program
int main(void) {
    initialize_todo_list();
    load_todos_from_file("todos.txt");

    int choice;
    char temp_description[MAX_DESCRIPTION_LENGTH];
    int temp_id;

    do {
        printf("\n--- Todo List Application ---\n");
        printf("1. Add Todo\n");
        printf("2. List Todos\n");
        printf("3. Edit Todo\n");
        printf("4. Mark Todo as Complete\n");
        printf("5. Delete Todo\n");
        printf("6. Save Todos\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1:
            printf("Enter the new todo description: ");
            fgets(temp_description, sizeof(temp_description), stdin);
            temp_description[strcspn(temp_description, "\n")] = 0;
            add_todo(temp_description);
            break;

        case 2:
            list_todos();
            break;

        case 3:
            printf("Enter the ID of the todo to edit: ");
            if (scanf("%d", &temp_id) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');
            printf("Enter the new description: ");
            fgets(temp_description, sizeof(temp_description), stdin);
            temp_description[strcspn(temp_description, "\n")] = 0;
            edit_todo_description(temp_id, temp_description);
            break;

        case 4:
            printf("Enter the ID of todo to mark complete: ");
            if (scanf("%d", &temp_id) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');
            complete_todo(temp_id);
            break;

        case 5:
            printf("Enter the ID of todo to delete: ");
            if (scanf("%d", &temp_id) != 1) {
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');
            delete_todo(temp_id);
            break;

        case 6:
            save_todos_to_file("todos.txt");
            break;

        case 7:
            printf("Exiting application. Have a great day!\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 7);

    free_todo_list();
    return 0;
}