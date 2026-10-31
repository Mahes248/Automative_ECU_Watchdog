/*
 * AUTOMOTIVE ECU WATCHDOG & RECOVERY FRAMEWORK
 * Team: CODE TITANS
 * QNX Momentics IDE 8.0.3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#define N 3
#define TIMEOUT 4.0
#define WAIT_NS ((_Uint64t)200000000ULL)
#define HEARTBEAT 1
#define COMMAND 2

typedef struct {
    int type;
    int data;
    char name[32];
    char command[32];
} Message;

typedef struct {
    char name[32];
    char cmd[16];
    pid_t pid;
    int data;
    int healthy;
    int recovering;
    int restarts;
    double last;
    double fault;
    double recovery;
} ECU;

ECU e[N] = {
    {"Battery", "battery", 0, 100, 0, 0, 0, 0, 0, 0},
    {"Motor Control", "motor", 0, 200, 0, 0, 0, 0, 0, 0},
    {"BMS", "bms", 0, 300, 0, 0, 0, 0, 0, 0}
};

char path[512];
char server[64];

double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return t.tv_sec + t.tv_nsec / 1e9;
}

int find(const char *s)
{
    int i;

    for (i = 0; i < N; i++) {
        if (!strcmp(s, e[i].name) ||
            !strcmp(s, e[i].cmd)) {
            return i;
        }
    }

    return -1;
}

pid_t start(int i)
{
    pid_t p;

    p = spawnl(P_NOWAIT, path, path,
               "--ecu", e[i].cmd, server, (char *)NULL);

    if (p == -1) {
        perror("ECU spawn");
        return -1;
    }

    e[i].pid = p;
    e[i].healthy = 0;
    e[i].last = now();

    return p;
}

void ecu_process(const char *name)
{
    int i, coid;
    Message m = {0};

    i = find(name);

    if (i < 0) {
        exit(EXIT_FAILURE);
    }

    coid = name_open(server, 0);

    if (coid == -1) {
        perror("ECU name_open");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        memset(&m, 0, sizeof(m));

        m.type = HEARTBEAT;
        strncpy(m.name, e[i].cmd, sizeof(m.name) - 1);
        m.data = e[i].data;

        if (MsgSend(coid, &m, sizeof(m), NULL, 0) == -1) {
            break;
        }

        sleep(1);
    }

    name_close(coid);
    exit(EXIT_SUCCESS);
}

void inject(int i)
{
    if (i < 0 || e[i].pid <= 0 ||
        e[i].recovering || !e[i].healthy) {
        printf("[ERROR] ECU unavailable or not ready.\n");
        return;
    }

    e[i].fault = now();

    if (kill(e[i].pid, SIGSTOP) == -1) {
        perror("Fault injection");
        return;
    }

    printf("\n[FAULT INJECTED] %s ECU\n", e[i].name);
    printf("[WATCHDOG] Waiting for heartbeat timeout...\n");
    fflush(stdout);
}

void recover(int i, double detected)
{
    double detection = detected - e[i].fault;
    pid_t old = e[i].pid;

    printf("\n[FAULT DETECTED] %s ECU heartbeat timeout\n",
           e[i].name);
    printf("[FAULT DETECTION DELAY] %.3f seconds\n", detection);
    printf("[RECOVERY STARTED] Restarting %s ECU only...\n",
           e[i].name);
    fflush(stdout);

    e[i].recovering = 1;
    e[i].healthy = 0;
    e[i].recovery = now();

    if (old > 0) {
        if (kill(old, SIGKILL) == -1 && errno != ESRCH) {
            perror("kill failed");
        }

        while (waitpid(old, NULL, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }

            if (errno != ECHILD) {
                perror("waitpid failed");
            }

            break;
        }
    }

    if (start(i) == -1) {
        printf("[RECOVERY FAILED] %s ECU could not restart.\n",
               e[i].name);

        e[i].recovering = 0;
        e[i].pid = -1;
        fflush(stdout);
        return;
    }

    e[i].restarts++;
}

void heartbeat(const Message *m)
{
    int i = find(m->name);
    double t;

    if (i < 0) {
        return;
    }

    t = now();

    e[i].last = t;
    e[i].healthy = 1;

    if (e[i].recovering) {
        printf("[RECOVERY SUCCESSFUL] %s ECU heartbeat restored.\n",
               e[i].name);
        printf("[RECOVERY DELAY] %.3f seconds\n",
               t - e[i].recovery);
        printf("[TOTAL RECOVERY TIME] %.3f seconds\n",
               t - e[i].fault);
        printf("[SYSTEM STATUS] Other ECU processes were not restarted.\n\n");

        e[i].recovering = 0;
        fflush(stdout);
    }
}

void watchdog(void)
{
    int i;
    double t = now();

    for (i = 0; i < N; i++) {
        if (e[i].pid > 0 && e[i].healthy &&
            !e[i].recovering &&
            t - e[i].last > TIMEOUT) {
            recover(i, t);
        }
    }
}

void cli(void)
{
    int coid, reply;
    char cmd[32];
    Message m = {0};

    coid = name_open(server, 0);

    if (coid == -1) {
        perror("CLI name_open");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        printf("\nEnter command (battery | motor | bms | status | exit): ");
        fflush(stdout);

        if (!fgets(cmd, sizeof(cmd), stdin)) {
            break;
        }

        cmd[strcspn(cmd, "\n")] = '\0';

        memset(&m, 0, sizeof(m));
        m.type = COMMAND;
        strncpy(m.command, cmd, sizeof(m.command) - 1);

        if (MsgSend(coid, &m, sizeof(m),
                    &reply, sizeof(reply)) == -1) {
            perror("CLI MsgSend");
            break;
        }

        if (!strcmp(cmd, "exit")) {
            break;
        }
    }

    name_close(coid);
    exit(EXIT_SUCCESS);
}

void supervisor(void)
{
    name_attach_t *a = name_attach(NULL, server, 0);
    Message m;
    int id, running = 1, reply = 1;

    if (!a) {
        perror("name_attach failed");
        exit(EXIT_FAILURE);
    }

    printf("\n====================================\n");
    printf(" AUTOMOTIVE ECU WATCHDOG & RECOVERY\n");
    printf(" TEAM: CODE TITANS\n");
    printf(" Heartbeat timeout: %.1f seconds\n", TIMEOUT);
    printf("====================================\n");

    for (int i = 0; i < N; i++) {
        if (start(i) == -1) {
            printf("[ERROR] Failed to start %s\n", e[i].name);
        }
    }

    if (spawnl(P_NOWAIT, path, path,
               "--cli", server, (char *)NULL) == -1) {
        perror("CLI spawn failed");
    }

    while (running) {
        _Uint64t timeout = WAIT_NS;

        TimerTimeout(CLOCK_MONOTONIC,
                     _NTO_TIMEOUT_RECEIVE,
                     NULL, &timeout, NULL);

        memset(&m, 0, sizeof(m));
        id = MsgReceive(a->chid, &m, sizeof(m), NULL);

        if (id == -1) {
            if (errno == ETIMEDOUT || errno == EINTR) {
                watchdog();
                continue;
            }

            perror("MsgReceive");
            break;
        }

        if (id == 0) {
            watchdog();
            continue;
        }

        if (m.type == HEARTBEAT) {
            heartbeat(&m);
            MsgReply(id, EOK, NULL, 0);
        }
        else if (m.type == COMMAND) {
            if (!strcmp(m.command, "status")) {
                for (int i = 0; i < N; i++) {
                    printf("[STATUS] %-14s: %-10s | Restarts: %d\n",
                           e[i].name,
                           e[i].recovering ? "RECOVERING" :
                           e[i].healthy ? "RUNNING" : "STARTING",
                           e[i].restarts);
                }
            }
            else if (!strcmp(m.command, "exit")) {
                MsgReply(id, EOK, &reply, sizeof(reply));
                running = 0;
                continue;
            }
            else {
                int i = find(m.command);

                if (i >= 0) {
                    inject(i);
                }
                else {
                    printf("[ERROR] Unknown command: %s\n", m.command);
                }
            }

            MsgReply(id, EOK, &reply, sizeof(reply));
        }

        watchdog();
    }

    printf("\n[SHUTDOWN] Stopping ECU processes...\n");

    for (int i = 0; i < N; i++) {
        if (e[i].pid > 0) {
            kill(e[i].pid, SIGKILL);

            while (waitpid(e[i].pid, NULL, 0) == -1 &&
                   errno == EINTR) {
            }
        }
    }

    name_detach(a, 0);
    printf("[SHUTDOWN] Complete.\n");
}

int main(int argc, char *argv[])
{
    if (argc > 1 && !strcmp(argv[1], "--ecu")) {
        if (argc < 4) {
            return EXIT_FAILURE;
        }

        strncpy(server, argv[3], sizeof(server) - 1);
        server[sizeof(server) - 1] = '\0';

        ecu_process(argv[2]);
        return EXIT_SUCCESS;
    }

    if (argc > 1 && !strcmp(argv[1], "--cli")) {
        if (argc < 3) {
            return EXIT_FAILURE;
        }

        strncpy(server, argv[2], sizeof(server) - 1);
        server[sizeof(server) - 1] = '\0';

        cli();
        return EXIT_SUCCESS;
    }

    if (!argv[0] || strlen(argv[0]) >= sizeof(path)) {
        fprintf(stderr, "Invalid executable path.\n");
        return EXIT_FAILURE;
    }

    strcpy(path, argv[0]);
    snprintf(server, sizeof(server), "ct_ecu_%ld", (long)getpid());

    supervisor();
    return EXIT_SUCCESS;
}
