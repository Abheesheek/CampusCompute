
#include <stdio.h>
#include <stdlib.h>

void display_menu(void)
{
    printf("\n========================================\n");
    printf("          CAMPUSCOMPUTE SYSTEM          \n");
    printf("========================================\n");
    printf("1. Job Management\n");
    printf("2. CPU Scheduling\n");
    printf("3. Concurrent Activity Logger\n");
    printf("4. Inter-Process Communication\n");
    printf("5. Banker's Safety Algorithm\n");
    printf("6. Worker Failure Detection\n");
    printf("7. Resource Limit Demonstration\n");
    printf("0. Exit\n");
    printf("========================================\n");
}

int main(void)
{
    int choice;

    while (1)
    {
        display_menu();
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Error: Invalid input. Please enter a number.\n");
            return EXIT_FAILURE;
        }

        switch (choice)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                printf("Feature %d is not implemented yet.\n", choice);
                break;

            case 0:
                printf("Exiting CampusCompute...\n");
                return EXIT_SUCCESS;

            default:
                printf("Error: Invalid menu choice.\n");
        }
    }
}
