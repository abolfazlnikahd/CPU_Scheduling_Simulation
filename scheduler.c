#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------- Scheduling Algorithms ----------------

// FCFS Scheduling
Metrics fcfs_metrics(Process proc[], int n) {
    Process processes[n];
    memcpy(processes, proc, sizeof(Process) * n);

    int currentTime = 0;
    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;

    // مرتب‌سازی بر اساس زمان ورود
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (processes[i].arrivalTime > processes[j].arrivalTime) {
                Process tmp = processes[i];
                processes[i] = processes[j];
                processes[j] = tmp;
            }

    for (int i = 0; i < n; ++i) {
        if (currentTime < processes[i].arrivalTime)
            currentTime = processes[i].arrivalTime;

        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        currentTime = processes[i].completionTime;

        int turnaround = processes[i].completionTime - processes[i].arrivalTime;
        int waiting = turnaround - processes[i].burstTime;
        int response = processes[i].startTime - processes[i].arrivalTime;

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
    }

    Metrics m;
    m.avgTurnaround = (float)totalTurnaround / n;
    m.avgWaiting = (float)totalWaiting / n;
    m.avgResponse = (float)totalResponse / n;
    return m;
}

// SJF Scheduling (Non-preemptive)
Metrics sjf_metrics(Process proc[], int n) {
    Process processes[n];
    memcpy(processes, proc, sizeof(Process) * n);

    int completed = 0, currentTime = 0;
    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;
    int isCompleted[n];
    memset(isCompleted, 0, sizeof(isCompleted));

    while (completed < n) {
        int idx = -1;
        int minBurst = 1e9;

        for (int i = 0; i < n; ++i) {
            if (!isCompleted[i] && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < minBurst ||
                    (processes[i].burstTime == minBurst && processes[i].arrivalTime < processes[idx].arrivalTime)) {
                    minBurst = processes[i].burstTime;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            currentTime++;
            continue;
        }

        processes[idx].startTime = currentTime;
        processes[idx].completionTime = currentTime + processes[idx].burstTime;
        currentTime = processes[idx].completionTime;

        int turnaround = processes[idx].completionTime - processes[idx].arrivalTime;
        int waiting = turnaround - processes[idx].burstTime;
        int response = processes[idx].startTime - processes[idx].arrivalTime;

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
        isCompleted[idx] = 1;
        completed++;
    }

    Metrics m;
    m.avgTurnaround = (float)totalTurnaround / n;
    m.avgWaiting = (float)totalWaiting / n;
    m.avgResponse = (float)totalResponse / n;
    return m;
}

// Round Robin Scheduling (Revised)
Metrics rr_metrics(Process proc[], int n, int timeQuantum) {
    Process processes[n];
    memcpy(processes, proc, sizeof(Process) * n);

    int remaining[n];
    int startTimes[n];
    int isStarted[n];
    for (int i = 0; i < n; ++i) {
        remaining[i] = processes[i].burstTime;
        isStarted[i] = 0;
        startTimes[i] = -1;
    }

    int currentTime = 0, completed = 0;
    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;

    int queue[1000];
    int front = 0, rear = 0;
    int visited[n];
    memset(visited, 0, sizeof(visited));

    while (completed < n) {
        // اضافه کردن پروسس‌هایی که وارد شده‌اند
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && !visited[i]) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }

        if (front == rear) {
            currentTime++;
            continue;
        }

        int idx = queue[front++];
        if (!isStarted[idx]) {
            startTimes[idx] = currentTime;
            isStarted[idx] = 1;
        }

        int execTime = (remaining[idx] < timeQuantum) ? remaining[idx] : timeQuantum;
        currentTime += execTime;
        remaining[idx] -= execTime;

        // اضافه کردن پروسس‌هایی که در این زمان وارد شده‌اند
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime > currentTime - execTime &&
                processes[i].arrivalTime <= currentTime &&
                !visited[i]) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }

        if (remaining[idx] > 0) {
            queue[rear++] = idx;  // بازگرداندن به صف
        } else {
            processes[idx].completionTime = currentTime;
            int turnaround = currentTime - processes[idx].arrivalTime;
            int waiting = turnaround - processes[idx].burstTime;
            int response = startTimes[idx] - processes[idx].arrivalTime;

            totalTurnaround += turnaround;
            totalWaiting += waiting;
            totalResponse += response;
            completed++;
        }
    }

    Metrics m;
    m.avgTurnaround = (float)totalTurnaround / n;
    m.avgWaiting = (float)totalWaiting / n;
    m.avgResponse = (float)totalResponse / n;
    return m;
}
