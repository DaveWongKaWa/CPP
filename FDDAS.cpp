#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <string>

using namespace std;

// -----------------------------------------------------------------------------------------------------
// Record the maximum length of T(title) and N(name) if it exceed 13
int maxTSize = 13;
int maxNSize = 13;

// Classes




// get essential information for a given valid time input (hh:mm);
class Time {
public:
    Time();
    Time(string);
    Time(int);

    string getFormat();
    int getValue();

    int cmp(Time);

private:
    string format;
    int hour;
    int minute;
    int value;
};

Time::Time() {

}

Time::Time(string _time) {
    format = _time;
    hour = (_time[0] - '0') * 10 + (_time[1] - '0');
    minute = (_time[3] - '0') * 10 + (_time[4] - '0');
    value = 60 * hour + minute;
}

Time::Time(int minutes) {
    hour = minutes / 60;
    minute = minutes % 60;

    if (hour < 10) format += "0";
    format += to_string(hour) + ":";
    if (minute < 10) format += "0";
    format += to_string(minute);

    value = minutes;
}

string Time::getFormat() {
    return format;
}

int Time::getValue() {
    return value;
}

int Time::cmp(Time other) {
    return value == other.value ? 0 : (value - other.value > 0 ? 1 : -1);
}




// A pair of time indicate a consecutive period
class TimeDuration {
public:
    TimeDuration();
    TimeDuration(string, string);
    TimeDuration(string, int);

    Time getStart();
    Time getEnd();

    int dist();
    double salary();
    int cmp(TimeDuration);

private:
    Time start;
    Time end;
};

TimeDuration::TimeDuration() {

}

TimeDuration::TimeDuration(string _start, string _end) {
    start = Time(_start);
    end = Time(_end);
}

TimeDuration::TimeDuration(string _start, int duration) {
    start = Time(_start);
    end = Time(start.getValue() + duration);
}

Time TimeDuration::getStart() {
    return start;
}

Time TimeDuration::getEnd() {
    return end;
}

int TimeDuration::dist() {
    return end.getValue() - start.getValue();
}

double TimeDuration::salary() {
    // [420, 720) -> $120 per hour, $2.0 per minute
    // [720, 840) -> $150 per hour, $2.5 per minute  i.e. lunch time
    // [840, 1080) -> $120 per hour, $2.0 per minute
    // [1080, 1200) -> $150 per hour, $2.5 per minute  i.e. dinner time
    int periodCut[5] = {420, 720, 840, 1080, 1200};
    int hourlyRate[4] = {120, 150, 120, 150};
    double minuteRate[4] = {2.0, 2.5, 2.0, 2.5};

    int startValue = start.getValue();
    int endValue = end.getValue();
    int length = 0;
 
    double totalSalary = 0;
    
    for (int i = 0; i < 4; ++ i) {
        if (startValue >= periodCut[i + 1] || endValue <= periodCut[i]) continue;
        length = min(endValue, periodCut[i + 1]) - max(startValue, periodCut[i]);
        totalSalary += (length / 60) * hourlyRate[i] + (length % 60) * minuteRate[i];
    }

    return totalSalary;
}

int TimeDuration::cmp(TimeDuration other) {
    // in our case, all duration are discrete (not overlapping), so we only have to compare the starting time
    return start.cmp(other.getStart());
}




// store the available time slots.
// if input duration is valid, divide the used slot to two unused slot
// e.g. insert [start, end] into [t1, t2] where t1 <= start && t2 >= end, result in [t1, start] and [t2, end]
class TimeLine {
public:
    TimeLine();
    TimeLine(string, string);

    void resize(int);
    void add(string, string);
    bool insert(TimeDuration);
    bool canInsert(TimeDuration);

    void sort();
    void show();
    void free();

private:
    Time lowerbound;
    Time upperbound;
    int idx;
    int size;
    TimeDuration* durations;
};

TimeLine::TimeLine() {

}

TimeLine::TimeLine(string _start, string _end) {
    lowerbound = Time(_start);
    upperbound = Time(_end);
    idx = 1;
    size = 2;
    durations = new TimeDuration[2];
    durations[0] = TimeDuration(_start, _end);
}

void TimeLine::resize(int newSize) {
    TimeDuration* tmp = new TimeDuration[newSize];
    for (int i = 0; i < idx; ++ i) {
        tmp[i] = durations[i];
    }
    delete[] durations;
    size = newSize;
    durations = tmp;
}

void TimeLine::add(string _start, string _end) {
    if (idx == size) resize(size << 1);
    durations[idx ++] = TimeDuration(_start, _end);
}

bool TimeLine::insert(TimeDuration period) {
    if (period.dist() == 0) return false;
    if ((period.getStart().cmp(lowerbound) < 0 || (period.getEnd()).cmp(upperbound) > 0)) return false; // out of bounds
    for (int i = 0; i < idx; ++ i) {
        if ((durations[i].getStart()).cmp(period.getStart()) <= 0 && (durations[i].getEnd()).cmp(period.getEnd()) >= 0) {
            string start1 = durations[i].getStart().getFormat();
            string end1 = period.getStart().getFormat();
            string start2 = period.getEnd().getFormat();
            string end2 = durations[i].getEnd().getFormat();

            durations[i] = TimeDuration(start1, end1);
            add(start2, end2);
            return true;
        } 
    }
    return false;
}

bool TimeLine::canInsert(TimeDuration period) {
    if (period.dist() == 0) return false;
    if ((period.getStart()).cmp(lowerbound) < 0 || (period.getEnd()).cmp(upperbound) > 0) return false; // out of bounds
    for (int i = 0; i < idx; ++ i) {
        if ((durations[i].getStart()).cmp(period.getStart()) <= 0 && (durations[i].getEnd()).cmp(period.getEnd()) >= 0) {
            return true;
        } 
    }
    return false;
}

void TimeLine::sort() {
    // selection sort
    TimeDuration* min;
    for (int i = 0; i < idx; ++ i) {
        min = &durations[i];
        for (int j = i + 1; j < idx; ++ j) {
            if (durations[j].cmp(*min) < 0) {
                min = &durations[j];
            }
        }
        TimeDuration tmp = durations[i];
        durations[i] = *min;
        *min = tmp;
    }
}

void TimeLine::show() {
    // show all available time slot(s), there may be time slot where the start time and end time are the same
    // e.g. allocate [start, end] to [t1, t2] where start == t1 && end == t2, result in two new time slots [t1, t1] and [t2, t2]
    // in this case, we do not consider time slot with distance (end - start) equal to 0
    sort();
    int numberOfLine = 1;
    for (int i = 0; i < idx; ++ i) {
        if (durations[i].dist() > 0) {
            cout << "[" << numberOfLine ++ << "] ";
            cout << durations[i].getStart().getFormat() << "-" << durations[i].getEnd().getFormat() << "\n";
        }
    }
}

void TimeLine::free() {
    delete[] durations;
}




// store single food order data
class FoodOrder {
public:
    FoodOrder();
    FoodOrder(string, string, int, int);

    string getTitle();
    string getTime();
    int getDuration();
    TimeDuration getProcessDuration();
    bool getAssign();

    void setAssign();

private:
    int id;
    string orderTitle;
    string pickupTime;
    int duration;
    TimeDuration processDuration;
    bool assigned;
};

FoodOrder::FoodOrder() {

}

FoodOrder::FoodOrder(string _title, string _time, int _duration, int _id) {
    id = _id;
    orderTitle = _title;
    pickupTime = _time;
    duration = _duration;
    processDuration = TimeDuration(_time, _duration);
    assigned = false;

    maxTSize = max(maxTSize, (int) orderTitle.length());
}

string FoodOrder::getTitle() {
    return orderTitle;
}

string FoodOrder::getTime() {
    return pickupTime;
}

int FoodOrder::getDuration() {
    return duration;
}

TimeDuration FoodOrder::getProcessDuration() {
    return processDuration;
}

bool FoodOrder::getAssign() {
    return assigned;
}

void FoodOrder::setAssign() {
    assigned = true;
}




// dynamic array implementation for FoodOrder class
// delete operation is not included
class FoodOrderList {
public:
    FoodOrderList();
    void resize(int);
    void add(string, string, int, int);
    FoodOrder* get(int);
    void show();
    void free();

private: 
    int idx;
    int size;
    FoodOrder* arr;
};

FoodOrderList::FoodOrderList() {
    idx = 0;
    size = 1;
    arr = new FoodOrder[1];
}

void FoodOrderList::resize(int newSize) {
    FoodOrder* tmp = new FoodOrder[newSize];
    for (int i = 0; i < idx; ++ i) {
        tmp[i] = arr[i];
    }
    delete[] arr;
    size = newSize;
    arr = tmp;
}

void FoodOrderList::add(string _title, string _time, int _duration, int _id) {
    if (idx == size) resize(size << 1);
    arr[idx ++] = FoodOrder(_title, _time, _duration, _id);
}

FoodOrder* FoodOrderList::get(int id) {
    return (id >= 0 && id < idx) ? &arr[id] : nullptr; 
}

void FoodOrderList::show() {
    cout << "\n";
    cout << string(79 + maxTSize, '-') << "\n";
    cout << setw(10) << "|OrderID" << setw(maxTSize + 3) << "|Order Title" << setw(18) << "|Food Pickup Time" << setw(29) << "|Delivery Duration (minutes)" << setw(19) << "|Assignment Status" << "|" << "\n";
    cout << string(79 + maxTSize, '-') << "\n";

    for (int i = 0; i < idx; ++ i) {
        cout << setw(10) << "|" + to_string(i) << setw(maxTSize + 3) << "|" + arr[i].getTitle() << setw(18) << "|" + arr[i].getTime() << setw(29) << "|" + to_string(arr[i].getDuration());
        cout << setw(19) << (arr[i].getAssign() ? "|Yes" : "|No") << "|" << "\n";
    }
    if (idx > 0) cout << string(79 + maxTSize, '-') << "\n";
}

void FoodOrderList::free() {
    delete[] arr;
}




// store single driver record
class DriverRecord {
public:
    DriverRecord();
    DriverRecord(string, string, string, int);

    double getSalary();
    string getDriverName();
    string getWorkingHourFrom();
    string getWorkingHourTo();
    TimeLine* getAvailables();

    void offerSalary(double);

    void show();
    void free();

private:
    int id;
    double salary;
    string driverName;
    string workingHourFrom;
    string workingHourTo;
    TimeLine availables;
};

DriverRecord::DriverRecord() {

}

DriverRecord::DriverRecord(string _name, string _from, string _to, int _id) {
    id = _id;
    salary = 0;
    driverName = _name;
    workingHourFrom = _from;
    workingHourTo = _to;
    availables = TimeLine(_from, _to);

    maxNSize = max(maxNSize, (int) driverName.length());
}

double DriverRecord::getSalary() {
    return salary;
}

string DriverRecord::getDriverName() {
    return driverName;
}

string DriverRecord::getWorkingHourFrom() {
    return workingHourFrom;
}

string DriverRecord::getWorkingHourTo() {
    return workingHourTo;
}

TimeLine* DriverRecord::getAvailables() {
    return &availables;
}

void DriverRecord::offerSalary(double additionalSalary) {
    salary += additionalSalary;
}

void DriverRecord::show() {
    cout << "\n";
    cout << "---------- Driver ---------" << "\n";
    cout << "DriverID: " << id << "\n";
    cout << "Driver's Name: " << driverName << "\n";
    cout << "Working Hour (From): " << workingHourFrom << "\n";
    cout << "Working Hour (To): " << workingHourTo << "\n";
    cout << "Driver's Free Timeslot(s): " << "\n";
    availables.show();
    cout << "Salary: $" << salary << "\n";
    cout << "----------------------------" << "\n";
}

void DriverRecord::free() {
    availables.free();
}




// dynamic array implementation for DriverRecord class
// delete operation is not included
class DriverRecordList {
public:
    DriverRecordList();

    void resize(int);
    void add(string, string, string, int);
    DriverRecord* get(int);

    void show();
    void free();

private:
    int idx;
    int size;
    DriverRecord* arr;
};

DriverRecordList::DriverRecordList() {
    idx = 0;
    size = 1;
    arr = new DriverRecord[1];
}

void DriverRecordList::resize(int newSize) {
    DriverRecord* tmp = new DriverRecord[newSize];
    for (int i = 0; i < idx; ++ i) {
        tmp[i] = arr[i];
    }
    delete[] arr;
    size = newSize;
    arr = tmp;
}

void DriverRecordList::add(string _name, string _from, string _to, int _id) {
    if (idx == size) resize(size << 1);
    arr[idx ++] = DriverRecord(_name, _from, _to, _id);
}

DriverRecord* DriverRecordList::get(int id) {
    return (id >= 0 && id < idx) ? &arr[id] : nullptr;
}

void DriverRecordList::show() {
    cout << "\n";
    cout << string(55 + maxNSize, '-') << "\n";
    cout << setw(10) << "|DriverID" << setw(maxNSize + 3) << "|Driver Name" << setw(21) << "|Working Hour (From)" << setw(21) << "|Working Hour (To)" << "|" << "\n";
    cout << string(55 + maxNSize, '-') << "\n";

    for (int i = 0; i < idx; ++ i) {
        cout << setw(10) << "|" + to_string(i) << setw(maxNSize + 3) << "|" + arr[i].getDriverName() << setw(21) << "|" + arr[i].getWorkingHourFrom() << setw(21) << "|" + arr[i].getWorkingHourTo() << "|" << "\n";
    }
    if (idx > 0) cout << string(55 + maxNSize, '-') << "\n";
}

void DriverRecordList::free() {
    for (int i = 0; i < idx; ++ i) {
        arr[i].free();
    }
    delete[] arr;
}




// -----------------------------------------------------------------------------------------------------

// the ID for order and driver
// when a single order / driver is added, assign the id to that order, and increment id by one
// it ensure unique id, and conveniently equal to its index in the array
int orderID = 0;
int driverID = 0;

// dynamic arrays
FoodOrderList orderList;
DriverRecordList driverList;

// determine when the program stop
bool on = true;

// -----------------------------------------------------------------------------------------------------
// cnt - use to determine the number of times of invalid inputs, terminate the process when cnt equal 3

// class::cmp(class) - use to determine the priority of between objects of the same class
// cmp -> 1 : greater than
// cmp -> 0 : equal
// cmp -> -1 : lesser than

// Functions

void f1(); // single input
void f2(); // batch input
void f3(); // show information
void f4(); // assign drivers
void f5(); // credits and exit

int main() {
    char main_opt;

    cout << left;
    cout << "Welcome Message designed by your group" << "\n";
    
    do {
        cout << "*** Main Menu ***" << "\n";
        cout << "[1] Single input" << "\n";
        cout << "[2] Batch input" << "\n";
        cout << "[3] Show information" << "\n";
        cout << "[4] Assign drivers" << "\n";
        cout << "[5] Credits and Exit" << "\n";
        cout << "*****************" << "\n";
        cout << "Option (1 - 5): ";

        do {
            cin >> main_opt;
            if (!cin) {
                cout << "Invalid option, please try again." << "\n";
                cin.clear();
                cin.ignore(256, '\n');
                continue;
            } 
            break;
        } while (true);
        
        switch(main_opt) {
            case '1': f1(); break;
            case '2': f2(); break;
            case '3': f3(); break;
            case '4': f4(); break;
            case '5': f5(); break;
            default: cout << "Invalid option, please try again." << "\n"; break;
        }
        cout << "\n";
    } while (on);

    orderList.free();
    driverList.free();
}




// -----------------------------------------------------------------------------------------------------

void addFoodOrder();
void addDriverRecord();

void f1() {
    int f1_opt;
    int cnt = 0;

    do {
        cout << "\n";
        cout << "*** Single Input Menu ***" << "\n";
        cout << "[1] Add a food order" << "\n";
        cout << "[2] Add a driver record" << "\n"; 
        cout << "Option (1 - 2): ";
        cin >> f1_opt;

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                cout << "Please enter 1 or 2. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur." << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
        } else {
            cin.ignore(256, '\n');
            switch(f1_opt) {
                case 1: addFoodOrder(); break;
                case 2: addDriverRecord(); break;
                default: {
                    ++ cnt;
                    if (cnt == 3) {
                        cout << "Process Terminate. " << "\n";
                    } else {
                        cout << "Please enter 1 or 2. ";
                        cout << "Process will terminate if " << 3 - cnt << " more invalid input occur." << "\n";
                    }
                    break;
                }
            }
        }
    } while (cnt < 3 && f1_opt != 1 && f1_opt != 2);
}

int valid(string time) {
    // 0: invalid format
    // 1: out of range
    // 2: valid
    if (time.size() > 5 || time[2] != ':') return 0;
    for (int i = 0; i < 5; ++ i) {
        if (i != 2 && (time[i] < '0' || time[i] > '9')) return 0;
    }

    int h, m;
    h = (time[0] - '0') * 10 + (time[1] - '0');
    m = (time[3] - '0') * 10 + (time[4] - '0');

    return m < 60 && ((h >= 7 && h < 20) || (h == 20 && m == 0)) ? 2 : 1; 
}

bool smaller(string time1, string time2) {
    // false: time1 >= time2
    // true: time1 < time2
    int h1, m1;
    int h2, m2;

    h1 = (time1[0] - '0') * 10 + (time1[1] - '0');
    m1 = (time1[3] - '0') * 10 + (time1[4] - '0');
    h2 = (time2[0] - '0') * 10 + (time2[1] - '0');
    m2 = (time2[3] - '0') * 10 + (time2[4] - '0');

    return h1 < h2 || (h1 == h2 && m1 < m2);
}

void addFoodOrder() {
    cout << "\n";

    char* buffer = new char[100];
    string title;
    string time;
    int duration;

    int cnt = 0;

    // input title
    do {
        cout << "Enter Order Title: ";
        cin.getline(buffer, 101, '\n');

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            }
            else {
                cout << "Input title is too long. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
            continue;
        } 
        for (int i = 0; i < 100; ++ i) {
            title += buffer[i];
        }
        break;
    } while (cnt < 3);
    delete[] buffer;
    if (cnt == 3) return;

    // input time
    cnt = 0;
    do {
        cout << "Enter Food Pickup Time (hh:mm): ";
        cin >> time;

        if (valid(time) != 2) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                switch(valid(time)) {
                    case 0: cout << "Input time does not satisfy the format (hh:mm). "; break;
                    case 1: cout << "Input time should be in the range of 07:00 to 20:00 inclusive. "; break;
                }
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            continue;
        } 
        break;
    } while (cnt < 3);
    if (cnt == 3) return;

    // input duration
    cnt = 0;
    do {
        cout << "Enter Delivery Duration (minutes): ";
        cin >> duration;

        if (!cin || duration <= 0 || duration >= 120) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                if (!cin) {
                    cout << "Input is not an integer. ";
                } else {
                    cout << "Input is not in the range of 1 to 119 inclusive. ";
                }
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            if (!cin) {
                cin.clear();
                cin.ignore(256, '\n');
            }
            continue;
        } 
        break;
    } while (cnt < 3);
    if (cnt == 3) return;

    // add food order into array and assign id
    orderList.add(title, time, duration, orderID);
    cout << "\n" << "The food order has been sucessfully added. ";
    cout << "The corresponding orderID is " << orderID ++ << "." << "\n";
}

void addDriverRecord() {
    cout << "\n";

    char* buffer = new char[100];
    string name;
    string hourFrom;
    string hourTo;

    int cnt = 0;

    // input driver name
    do {
        cout << "Enter Driver Name: ";
        cin.getline(buffer, 101, '\n');

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                cout << "Input name is too long. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
            continue;
        } 
        for (int i = 0; i < 100; ++ i) {
            name += buffer[i];
        }
        break;
    } while (cnt < 3);
    delete[] buffer;
    if (cnt == 3) return;

    // input working hour from
    cnt = 0;
    do {
        cout << "Enter Working Hour (From) (hh:mm): ";
        cin >> hourFrom;

        if (valid(hourFrom) != 2) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                switch(valid(hourFrom)) {
                    case 0: cout << "Input time does not satisfy the format (hh:mm). "; break;
                    case 1: cout << "Input time should be in the range of 07:00 to 20:00 inclusive. "; break;
                }
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            continue;
        }
        break;
    } while (cnt < 3);
    if (cnt == 3) return;

    // input working hour to
    cnt = 0;
    do {
        cout << "Enter Working Hour (To) (hh:mm): ";
        cin >> hourTo;

        if (valid(hourTo) != 2 || !smaller(hourFrom, hourTo)) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                switch(valid(hourTo)) {
                    case 0: cout << "Input time does not satisfy the format (hh:mm). "; break;
                    case 1: cout << "Input time should be in the range of 07:00 to 20:00 inclusive. "; break;
                    case 2: cout << "Working Hour (To) should be greater than Working Hour (From). "; break;
                }
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            continue;
        } 
        break;
    } while (cnt < 3);
    if (cnt == 3) return;

    // add driver into array and assign id
    driverList.add(name, hourFrom, hourTo, driverID);
    cout << "\n" << "The driver record has been sucessfully added. ";
    cout << "The corresponding driverID is " << driverID ++ << "." << "\n";
}




// -----------------------------------------------------------------------------------------------------
void addMultipleFoodOrders();
void addMultipleDriverRecords();

void f2() {
    int f2_opt;
    int cnt = 0;

    do {
        cout << "\n";
        cout << "*** Batch Input Menu ***" << "\n";
        cout << "[1] Add food orders" << "\n";
        cout << "[2] Add driver records" << "\n"; 
        cout << "Option (1 - 2): ";
        cin >> f2_opt;

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                cout << "Please enter 1 or 2. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
        } else {
            cin.ignore(256, '\n');
            switch(f2_opt) {
                case 1: addMultipleFoodOrders(); break;
                case 2: addMultipleDriverRecords(); break;
                default: {
                    ++ cnt;
                    if (cnt == 3) {
                        cout << "Process Terminate. " << "\n";
                    } else {
                        cout << "Please enter 1 or 2. ";
                        cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
                    }
                    break;
                }
            }
        }
    } while (cnt < 3 && f2_opt != 1 && f2_opt != 2);
}


void addMultipleFoodOrders() {
    cout << "\n";
    cout << "--- Food Order(s) ---" << "\n";
    // at most 100(title) + 5(hh:mm) + 3(119) + 2('/') == 110 long
    int stage;
    int orderCnt = 0;
    char* buffer = new char[110];

    while (true) {
        string orderTitle;
        string time;
        int duration = 0;

        stage = 0;
        cin.getline(buffer, 111, '\n');
        
        if (strlen(buffer) == 1 && buffer[0] == '0') break;


        for (int i = 0; i < 111 && buffer[i] != '\0'; ++ i) {
            if (buffer[i] == '/') {
                buffer[i] = '\0';
                ++ stage;
                continue;
            }
            switch(stage) {
                case 0: orderTitle += buffer[i]; break;
                case 1: time += buffer[i]; break;
                case 2: duration = 10 * duration + buffer[i] - '0'; break;
            }
            if (stage) buffer[i] = '\0';
        }

        ++ orderCnt;
        orderList.add(orderTitle, time, duration, orderID ++);
    }

    delete[] buffer;
    cout << left;
    cout << setw(20) << "Food orders: " << orderCnt << " food order(s) added" << "\n";    
}

void addMultipleDriverRecords() {
    cout << "\n";
    cout << "--- Driver Record(s) ---" << "\n";
    // at most 100(title) + 5(hh:mm) + 5(hh:mm) + 2('/') == 112 long
    int stage;
    int recordCnt = 0;
    char* buffer = new char[112];

    while (true) {
        string driverName;
        string startTime;
        string endTime;

        stage = 0;
        cin.getline(buffer, 113, '\n');
        
        if (strlen(buffer) == 1 && buffer[0] == '0') break;


        for (int i = 0; i < 111 && buffer[i] != '\0'; ++ i) {
            if (buffer[i] == '/') {
                buffer[i] = '\0';
                ++ stage;
                continue;
            }
            switch(stage) {
                case 0: driverName += buffer[i]; break;
                case 1: startTime += buffer[i]; break;
                case 2: endTime += buffer[i]; break;
            }
            if (stage) buffer[i] = '\0';
        }

        ++ recordCnt;
        driverList.add(driverName, startTime, endTime, driverID ++);
    }

    delete[] buffer;
    cout << left;
    cout << setw(20) << "Driver records: " << recordCnt << " driver(s) added" << "\n";    
}




// -----------------------------------------------------------------------------------------------------

void showAllFoodOrders();
void showAllDrivers();
void showOneDriverByID();

void f3() {
    int f3_opt;
    int cnt = 0;

    do {
        cout << "\n";
        cout << "***** Show Info Menu *****" << "\n";
        cout << "[1] Show all food orders" << "\n";
        cout << "[2] Show all drivers" << "\n";
        cout << "[3] Show one driver by ID" << "\n";
        cout << "[4] Return to Main Menu" << "\n";
        cout << "Option (1 - 4): ";
        cin >> f3_opt;

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                cout << "Invalid option, please try again. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
        } else {
            cin.ignore(256, '\n');
            switch(f3_opt) {
                case 1: showAllFoodOrders(); break;
                case 2: showAllDrivers(); break;
                case 3: showOneDriverByID(); break;
                case 4: break;
                default: {
                    ++ cnt;
                    if (cnt == 3) {
                        cout << "Process Terminate. " << "\n";
                    } else {
                        cout << "Invalid option, please try again. ";
                        cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
                    }
                }
            }
        }
    } while (!(f3_opt > 0 && f3_opt < 5) && cnt < 3);
}

void showAllFoodOrders() {
    orderList.show();
}

void showAllDrivers() {
    driverList.show();
}

void showOneDriverByID() {
    int id;

    cout << "\n";
    cout << "Driver ID: ";
    cin >> id;
    if (!driverList.get(id)) {
        cout << "The driver correspond to the ID<" << id << "> does not exist. " << "\n";
        return;
    }
    (*driverList.get(id)).show();
}




// -----------------------------------------------------------------------------------------------------

void manualAssign();
void autoAssign();

void f4() {
    int f4_opt;
    int cnt = 0;

    do {
        cout << "\n";
        cout << "**** Assign Drivers Menu ****" << "\n";
        cout << "[1] Assign drivers manually" << "\n";
        cout << "[2] Assign drivers automatically" << "\n";
        cout << "[3] Return to Main Menu" << "\n";
        cout << "*****************************" << "\n";
        cout << "Option (1 - 3): ";
        cin >> f4_opt;

        if (!cin) {
            ++ cnt;
            if (cnt == 3) {
                cout << "Process Terminate. " << "\n";
            } else {
                cout << "Invalid option, please try again. ";
                cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
            }
            cin.clear();
            cin.ignore(256, '\n');
        } else {
            switch(f4_opt) {
                case 1: manualAssign(); break;
                case 2: autoAssign(); break;
                case 3: break;
                default: {
                    ++ cnt;
                    if (cnt == 3) {
                        cout << "Process Terminate. " << "\n";
                    } else {
                        cout << "Invalid option, please try again. ";
                        cout << "Process will terminate if " << 3 - cnt << " more invalid input occur. " << "\n";
                    }
                    break;
                }
            }
        }
    } while (f4_opt != 3 && cnt < 3);
}

void manualAssign() {
    int id;

    cout << "\n";
    cout << "Driver ID: ";
    cin >> id;
    cin.ignore(256, '\n');

    if (!driverList.get(id)) {
        cout << "The driver correspond to the id<" << id << "> does not exist. " << "\n";
        return;
    }

    DriverRecord* driver = driverList.get(id);
    TimeLine* slots = (*driver).getAvailables();
    FoodOrder* order;

    int maxInputLength = 0;
    int orderCnt = 0;

    cout << "Driver's Name: " << (*driver).getDriverName() << "\n";
    cout << "Driver's Free Timeslot(s): " << "\n";
    (*slots).show();


    bool canHandleSomeOrder = false;
    for (int i = 0; i < orderID; ++ i) {
        order = orderList.get(i);
        if ((*order).getAssign()) continue;
        if ((*slots).canInsert((*order).getProcessDuration())) {
            canHandleSomeOrder = true; break;
        }
    }
    

    if (canHandleSomeOrder) {
        cout << "Food Orders that can be handled: " << "\n";
        cout << string(60 + maxTSize, '-') << "\n";
        cout << setw(10) << "|OrderID" << setw(maxTSize + 3) << "|Order Title" << setw(18) << "|Food Pickup Time" << setw(29) << "|Delivery Duration (minutes)" << "|" << "\n";
        cout << string(60 + maxTSize, '-') << "\n";

        for (int i = 0; i < orderID; ++ i) {
            order = orderList.get(i);
            if ((*order).getAssign()) continue;
            if ((*slots).canInsert((*order).getProcessDuration())) {
                cout << setw(10) << "|" + to_string(i) << setw(maxTSize + 3) << "|" + (*order).getTitle() << setw(18) << "|" + (*order).getTime() << setw(29) << "|" + to_string((*order).getDuration()) << "|" << "\n";
                maxInputLength += (int) (log10(i) + 2);
                ++ orderCnt;
            }
        }
        cout << string(60 + maxTSize, '-') << "\n";
    } else {
        cout << "No food order can be handled. " << "\n";
        return;
    }


    int curID = 0;
    char* buffer = new char[maxInputLength];

    cout << "Food Orders' ID: ";
    cin.getline(buffer, maxInputLength + 1, '\n');

    cout << "\n";
    cout << "The Following Food Orders have been sucessfully assigned. " << "\n";
    cout << string(60 + maxTSize, '-') << "\n";
    cout << setw(10) << "|OrderID" << setw(maxTSize + 3) << "|Order Title" << setw(18) << "|Food Pickup Time" << setw(29) << "|Delivery Duration (minutes)" << "|" << "\n";
    cout << string(60 + maxTSize, '-') << "\n";

    bool haveHandledSomeOrder = false;
    for (int i = 0, j = -1; i <= maxInputLength; ++ i) {
        if (buffer[i] == ' ' || buffer[i] == '\0') {
            for (int k = j + 1; k < i; ++ k) {
                curID = 10 * curID + buffer[k] - '0';
            }
            if (curID < 0 || curID >= orderID) break;

            order = orderList.get(curID);

            if ((*order).getAssign() || !(*slots).insert((*order).getProcessDuration())) break;

            (*driver).offerSalary((*order).getProcessDuration().salary());
            (*order).setAssign();
            haveHandledSomeOrder = true;
            cout << setw(10) << "|" + to_string(curID) << setw(maxTSize + 3) << "|" + (*order).getTitle() << setw(18) << "|" + (*order).getTime() << setw(29) << "|" + to_string((*order).getDuration()) << "|" << "\n";

            j = i;
            curID = 0;
        }
        if (buffer[i] == '\0') break;
    }
    if (haveHandledSomeOrder) cout << string(60 + maxTSize, '-') << "\n";
    delete[] buffer;

    cout << "The Updated Driver Status: " << "\n";    
    (*driver).show();
}

void autoAssign() {
    FoodOrder* order;
    DriverRecord* driver;


    cout << "\n";
    cout << "The Following Food Orders have been automatically assigned. " << "\n";
    cout << string(73 + maxTSize + maxNSize, '-') << "\n";
    cout << setw(10) << "|OrderID" << setw(maxTSize + 3) << "|Order Title" << setw(18) << "|Food Pickup Time" << setw(29) << "|Delivery Duration (minutes)";
    cout << setw(10) << "|DriverID" << setw(maxNSize + 3) << "|Driver Name" << "|" << "\n";
    cout << string(73 + maxTSize + maxNSize, '-') << "\n";

    bool haveHandledSomeOrder = false;
    for (int i = 0; i < orderID; ++ i) {
        order = orderList.get(i);
        for (int j = 0; j < driverID && !(*order).getAssign(); ++ j) {
            driver = driverList.get(j);
            if ((*(*driver).getAvailables()).insert((*order).getProcessDuration())) {
                (*driver).offerSalary((*order).getProcessDuration().salary());
                (*order).setAssign();
                haveHandledSomeOrder = true;
                cout << setw(10) << "|" + to_string(i) << setw(maxTSize + 3) << "|" + (*order).getTitle() << setw(18) << "|" + (*order).getTime() << setw(29) << "|" + to_string((*order).getDuration());
                cout << setw(10) << "|" + to_string(j) << setw(maxNSize + 3) << "|" + (*driver).getDriverName() << "|" << "\n";
            }
        }
    }
    if (haveHandledSomeOrder) cout << string(73 + maxTSize + maxNSize, '-') << "\n";


    cout << "\n";
    cout << "The Following Food Orders have not yet been assigned. " << "\n";
    cout << string(60 + maxTSize, '-') << "\n";
    cout << setw(10) << "|OrderID" << setw(maxTSize + 3) << "|Order Title" << setw(18) << "|Food Pickup Time" << setw(29) << "|Delivery Duration (minutes)" << "|" << "\n";
    cout << string(60 + maxTSize, '-') << "\n";

    bool haveSomeUnhandledOrder = false;
    for (int i = 0; i < orderID; ++ i) {
        order = orderList.get(i);
        if (!(*order).getAssign()) {
            haveSomeUnhandledOrder = true;
            cout << setw(10) << "|" + to_string(i) << setw(maxTSize + 3) << "|" + (*order).getTitle() << setw(18) << "|" + (*order).getTime() << setw(29) << "|" + to_string((*order).getDuration()) << "|" << "\n";
        }
    }
    if (haveSomeUnhandledOrder) cout << string(60 + maxTSize, '-') << "\n";
}




// -----------------------------------------------------------------------------------------------------

void credit();

void f5() {
    char f5_opt;

    cout << "\n";

    while (f5_opt != 'n' && f5_opt != 'N' && f5_opt != 'y' && f5_opt != 'Y') {
        cout << "Exit (y/Y) | Back To Menu (n/N): ";
        cin >> f5_opt;

        switch(f5_opt) {
            case 'n': break;
            case 'N': break;
            case 'y': credit(); break;
            case 'Y': credit(); break;
            default: cout << "Please enter (y / Y / n / N). " << "\n"; break;
        }
    } 
}

void credit() {
    cout << "\n"; 
    cout << string(51, '-') << "\n";
    cout << setw(15) << "|Student Name" << setw(14) << "|Student ID" << setw(8) << "|Class" << setw(15) << "|Tutorial Group" << "|" << "\n";
    cout << string(51, '-') << "\n";
    cout << setw(15) << "|Wong Ka Wa" << setw(14) << "|20199071A" << setw(8) << "|101" << setw(15) << "|B" << "|" << "\n";
    cout << setw(15) << "|Tung Chun Hei" << setw(14) << "|20093934A" << setw(8) << "|101" << setw(15) << "|B" << "|" << "\n";
    cout << setw(15) << "|Cheung Ho Wai" << setw(14) << "|20029137A" << setw(8) << "|101" << setw(15) << "|B" << "|" << "\n";
    cout << setw(15) << "|Tse Kong Hin" << setw(14) << "|20211562A" << setw(8) << "|101" << setw(15) << "|B" << "|" << "\n";
    cout << setw(15) << "|Chan Lok Hin" << setw(14) << "|20032074A" << setw(8) << "|101" << setw(15) << "|B" << "|" << "\n";
    cout << string(51, '-') << "\n";

    cout << "Thank you for using our service. " << "\n";
    on = false;
}
