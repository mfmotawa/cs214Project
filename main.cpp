#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

/*========================================
=                 Process                =
========================================*/
struct Process {
    int id, burst, priority;
    string name;
};

/*========================================
=               LinkedList               =
========================================*/
struct ListNode {
    Process p;
    ListNode* next;
};

class LinkedList {
    ListNode* head;

public:
    LinkedList() : head(NULL) {}

    void add(const Process& pr) {
        ListNode* n = new ListNode{ pr, NULL };
        if (!head) head = n;
        else {
            ListNode* t = head;
            while (t->next) t = t->next;
            t->next = n;
        }
    }

    bool remove(int id) {
        if (!head) return false;

        if (head->p.id == id) {
            ListNode* tmp = head;
            head = head->next;
            delete tmp;
            return true;
        }

        ListNode* t = head;
        while (t->next) {
            if (t->next->p.id == id) {
                ListNode* del = t->next;
                t->next = del->next;
                delete del;
                return true;
            }
            t = t->next;
        }
        return false;
    }

    bool edit(int id, const Process& np) {
        ListNode* t = head;
        while (t) {
            if (t->p.id == id) {
                t->p.name = np.name;
                t->p.burst = np.burst;
                t->p.priority = np.priority;
                return true;
            }
            t = t->next;
        }
        return false;
    }

    void print() const {
        cout << "\nID   Name   Burst   Priority\n";
        cout << "-----------------------------\n";

        if (!head) {
            cout << "No processes.\n";
            return;
        }

        ListNode* t = head;
        while (t) {
            cout << t->p.id << "    "
                << t->p.name << "    "
                << t->p.burst << "       "
                << t->p.priority << "\n";
            t = t->next;
        }
    }

    void load(const string& filename) {
        ifstream in(filename);
        if (!in.is_open()) {
            cout << "Cannot open file.\n";
            return;
        }

        Process p;
        while (in >> p.id >> p.name >> p.burst >> p.priority)
            add(p);

        cout << "Loaded from " << filename << "\n";
    }

    void save(const string& filename) const {
        ofstream out(filename);
        ListNode* t = head;

        while (t) {
            out << t->p.id << " "
                << t->p.name << " "
                << t->p.burst << " "
                << t->p.priority << "\n";
            t = t->next;
        }

        cout << "Saved to " << filename << "\n";
    }

    ListNode* getHead() const { return head; }
};

/*========================================
=          LinkedList Queue             =
========================================*/
class Queue {
    struct QNode {
        Process p;
        QNode* next;
    };

    QNode* front;
    QNode* rear;

public:
    Queue() : front(NULL), rear(NULL) {}

    bool empty() const { return front == NULL; }

    void enqueue(const Process& p) {
        QNode* n = new QNode{ p, NULL };
        if (!rear)
            front = rear = n;
        else
            rear->next = n, rear = n;
    }

    Process dequeue() {
        QNode* t = front;
        Process p = t->p;

        front = front->next;
        if (!front) rear = NULL;

        delete t;
        return p;
    }
};

/*========================================
=         Priority Queue (Array)         =
========================================*/
class PriorityQueue {
    struct Item {
        Process p{};
        int pri{};
    };

    Item arr[200];
    int size;

public:
    PriorityQueue() : size(0) {}

    bool empty() const { return size == 0; }

    void enqueue(const Process& p) {
        int i = size - 1;
        while (i >= 0 && arr[i].pri > p.priority) {
            arr[i + 1] = arr[i];
            i--;
        }
        arr[i + 1].p = p;
        arr[i + 1].pri = p.priority;
        size++;
    }

    Process dequeue() {
        Process top = arr[0].p;

        for (int i = 0; i < size - 1; i++)
            arr[i] = arr[i + 1];

        size--;
        return top;
    }
};

/*========================================
=          Priority Scheduling           =
========================================*/
void runScheduling(LinkedList& list) {
    Queue ready;

    for (ListNode* t = list.getHead(); t; t = t->next)
        ready.enqueue(t->p);

    PriorityQueue pq;
    while (!ready.empty())
        pq.enqueue(ready.dequeue());

    ofstream out("priority_output.txt");
    if (!out.is_open()) {
        cout << "Cannot open output file.\n";
        return;
    }

    out << left << setw(6) << "ID"
        << setw(10) << "Name"
        << setw(8) << "Burst"
        << setw(10) << "Priority"
        << setw(10) << "Waiting"
        << setw(12) << "Turnaround" << "\n";

    out << "--------------------------------------------------------------\n";

    int time = 0;
    int totalWait = 0, totalTurn = 0, count = 0;

    while (!pq.empty()) {
        Process p = pq.dequeue();

        int wait = time;
        int turn = time + p.burst;

        out << left << setw(6) << p.id
            << setw(10) << p.name
            << setw(8) << p.burst
            << setw(10) << p.priority
            << setw(10) << wait
            << setw(12) << turn << "\n";

        totalWait += wait;
        totalTurn += turn;
        count++;

        time += p.burst;
    }

    if (count > 0) {
        out << "\nAverage Waiting Time: " << (double)totalWait / count << "\n";
        out << "Average Turnaround Time: " << (double)totalTurn / count << "\n";
    }

    cout << "Scheduling done -> priority_output.txt\n";
}

/*========================================
=                  MAIN                  =
========================================*/
int main() {
    LinkedList list;
    list.load("processes.txt");

    int ch;

    while (true) {
        cout << "\n=== CPU Manager ===\n"
            << "1. Add\n"
            << "2. Edit\n"
            << "3. Delete\n"
            << "4. Print\n"
            << "5. Schedule\n"
            << "6. Save\n"
            << "7. Exit\n"
            << "Choose: ";

        cin >> ch;

        if (ch == 1) {
            Process p;
            cout << "ID: ";       cin >> p.id;
            cout << "Name: ";     cin >> p.name;
            cout << "Burst: ";    cin >> p.burst;
            cout << "Priority: "; cin >> p.priority;
            list.add(p);
        }
        else if (ch == 2) {
            int id;
            cout << "ID: ";
            cin >> id;

            Process np;
            np.id = id;

            cout << "New Name: ";     cin >> np.name;
            cout << "New Burst: ";    cin >> np.burst;
            cout << "New Priority: "; cin >> np.priority;

            if (!list.edit(id, np))
                cout << "Not found\n";
        }
        else if (ch == 3) {
            int id;
            cout << "ID: ";
            cin >> id;

            if (!list.remove(id))
                cout << "Not found\n";
        }
        else if (ch == 4) {
            list.print();
        }
        else if (ch == 5) {
            runScheduling(list);
        }
        else if (ch == 6) {
            list.save("processes.txt");
        }
        else if (ch == 7) {
            break;
        }
        else {
            cout << "Invalid!\n";
        }
    }

    return 0;
}
