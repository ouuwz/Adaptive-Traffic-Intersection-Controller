
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_GREEN 10
#define MAX_GREEN 60
#define FIXED_GREEN 30
#define SIMULATION_TIME 300

struct Approach {
    int id;
    char name[20];
    int vehicles;
    int queueLength;
    int greenTime;
    int passed;
    float totalWait;
};

struct Event {
    int time;
    int phase;
    int vehiclesPassed;
};

struct Approach *traffic[10];
struct Event logData[1000];

int count = 0, eventCount = 0;

void addApproach() {
    if (count >= 10) {
        printf("\nMaximum approaches reached!\n");
        return;
    }

    traffic[count] = (struct Approach *)malloc(sizeof(struct Approach));

    if (traffic[count] == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    struct Approach *p = traffic[count];

    p->id = count + 1;
    printf("Enter approach name: ");
    scanf("%19s", p->name);

    printf("Enter initial vehicle count: ");
    scanf("%d", &p->vehicles);

    if (p->vehicles < 0) p->vehicles = 0;

    p->queueLength = p->vehicles;
    p->greenTime = FIXED_GREEN;
    p->passed = 0;
    p->totalWait = 0;

    count++;

    printf("\nApproach added successfully!\n");
}

void removeApproach() {
    int id;

    if (count == 0) {
        printf("No approaches available!\n");
        return;
    }

    printf("Enter approach ID to remove: ");
    scanf("%d", &id);

    if (id < 1 || id > count) {
        printf("Invalid ID!\n");
        return;
    }

    free(traffic[id - 1]);

    for (int i = id - 1; i < count - 1; i++) {
        traffic[i] = traffic[i + 1];
        traffic[i]->id = i + 1;
    }

    count--;
    printf("Approach removed successfully!\n");
}

void displayTraffic() {
    printf("\n========== TRAFFIC STATUS ==========\n");

    printf("%-5s %-15s %-12s %-12s\n",
           "ID", "Approach", "Vehicles", "Queue");

    for (int i = 0; i < count; i++) {
        struct Approach *p = traffic[i];

        printf("%-5d %-15s %-12d %-12d\n",
               p->id, p->name, p->vehicles, p->queueLength);
    }
}

void calculateAdaptiveTiming() {
    int totalQueue = 0;

    for (int i = 0; i < count; i++)
        totalQueue += traffic[i]->queueLength;

    for (int i = 0; i < count; i++) {
        if (totalQueue == 0) {
            traffic[i]->greenTime = MIN_GREEN;
        } else {
            traffic[i]->greenTime =
                (traffic[i]->queueLength * 100) / totalQueue;

            if (traffic[i]->greenTime < MIN_GREEN)
                traffic[i]->greenTime = MIN_GREEN;

            if (traffic[i]->greenTime > MAX_GREEN)
                traffic[i]->greenTime = MAX_GREEN;
        }
    }
}

void simulate(int adaptive) {
    if (count == 0) {
        printf("\nAdd approaches first!\n");
        return;
    }

    int totalPassed = 0;
    float totalWait = 0;

    for (int i = 0; i < count; i++) {
        traffic[i]->passed = 0;
        traffic[i]->totalWait = 0;
    }

    printf("\nStarting %s Traffic Simulation...\n",
           adaptive ? "ADAPTIVE" : "FIXED-TIME");

    for (int time = 0; time < SIMULATION_TIME; time += 10) {

        if (adaptive)
            calculateAdaptiveTiming();

        for (int i = 0; i < count; i++) {
            struct Approach *p = traffic[i];

            int green = adaptive ? p->greenTime : FIXED_GREEN;

            int passed = (green / 10) * 3;

            if (passed > p->vehicles)
                passed = p->vehicles;

            p->vehicles -= passed;
            p->queueLength = p->vehicles;

            p->passed += passed;
            totalPassed += passed;

            p->totalWait += p->vehicles * 10;

            if (eventCount < 1000) {
                logData[eventCount].time = time;
                logData[eventCount].phase = i + 1;
                logData[eventCount].vehiclesPassed = passed;
                eventCount++;
            }
        }

        // New vehicles arrive from simulated sensors
        for (int i = 0; i < count; i++) {
            int arrival = rand() % 4;
            traffic[i]->vehicles += arrival;
            traffic[i]->queueLength = traffic[i]->vehicles;
        }
    }

    for (int i = 0; i < count; i++)
        totalWait += traffic[i]->totalWait;

    printf("\n========== SIMULATION RESULTS ==========\n");
    printf("Strategy          : %s\n",
           adaptive ? "Adaptive" : "Fixed-Time");
    printf("Simulation time   : %d seconds\n", SIMULATION_TIME);
    printf("Total vehicles passed: %d\n", totalPassed);
    printf("Total waiting time   : %.2f vehicle-seconds\n", totalWait);
    printf("Average wait time    : %.2f seconds\n",
           totalPassed ? totalWait / totalPassed : 0);

    printf("\nApproach-wise Results:\n");

    for (int i = 0; i < count; i++) {
        printf("%-15s Passed: %-5d Remaining: %d\n",
               traffic[i]->name,
               traffic[i]->passed,
               traffic[i]->vehicles);
    }

    FILE *fp = fopen("traffic_results.txt", "a");

    if (fp != NULL) {
        fprintf(fp, "\nStrategy: %s\n",
                adaptive ? "Adaptive" : "Fixed-Time");
        fprintf(fp, "Vehicles Passed: %d\n", totalPassed);
        fprintf(fp, "Average Wait: %.2f seconds\n",
                totalPassed ? totalWait / totalPassed : 0);
        fclose(fp);
        printf("\nResults saved to traffic_results.txt\n");
    }
}

void showEventLog() {
    printf("\n========== EVENT LOG ==========\n");

    if (eventCount == 0) {
        printf("No events recorded yet!\n");
        return;
    }

    for (int i = 0; i < eventCount; i++) {
        printf("Time: %3d sec | Approach: %d | Vehicles Passed: %d\n",
               logData[i].time,
               logData[i].phase,
               logData[i].vehiclesPassed);
    }
}

void showMenu() {
    printf("\n====================================\n");
    printf("   ADAPTIVE TRAFFIC CONTROLLER\n");
    printf("====================================\n");
    printf("1. Add Traffic Approach\n");
    printf("2. Remove Traffic Approach\n");
    printf("3. Display Traffic Status\n");
    printf("4. Run Adaptive Simulation\n");
    printf("5. Run Fixed-Time Simulation\n");
    printf("6. View Event Log\n");
    printf("7. Exit\n");
    printf("------------------------------------\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;

    srand(42);

    while (1) {
        showMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            break;
        }

        switch (choice) {
            case 1:
                addApproach();
                break;

            case 2:
                removeApproach();
                break;

            case 3:
                displayTraffic();
                break;

            case 4:
                simulate(1);
                break;

            case 5:
                simulate(0);
                break;

            case 6:
                showEventLog();
                break;

            case 7:
                for (int i = 0; i < count; i++)
                    free(traffic[i]);

                printf("\nTraffic Controller Closed.\n");
                return 0;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    for (int i = 0; i < count; i++)
        free(traffic[i]);

    return 0;
}
