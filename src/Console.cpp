#include "Console.h"
#include "input.hpp"
#include "ctime"
#include "Date.h"

void Console::test(std::string text) 
{ 
    std::cout << text << " in development" << std::endl;
}

void Console::displayMenu()
{
    WriteLine( "Medical City real");
    WriteLine( "-----------------");
    WriteLine( "[1] Patients     ");
    WriteLine( "[2] Doctors      ");
    WriteLine( "[3] Inventory    ");
    WriteLine( "[4] Md Records   "); 
    WriteLine( "[5] add Billing  ");
    WriteLine( "[6] Schedule     ");
    WriteLine( "[7] Message      ");
    WriteLine( "[999] quit       ");
    WriteLine( "-----------------");
}

void Console::patients()
{
    WriteLine("--------Patients---------");
    WriteLine("[1] Add new patient      ");
    WriteLine("[2] View all             ");
    WriteLine("[3] search               ");
    WriteLine("[4] update               ");
    WriteLine("\n[5] ->Back to Main Menu");
    WriteLine("-------------------------");

    int choice = 0; cin.ignore();
    getNumber("Enter Choice: ", choice, MinMax(1,-1));

    Clear();

    switch (choice)
    {
        case 1: addPatient();
            break;
        case 2: viewPatients();
            break;
        case 3: findPatient();
            break;
        case 4: updatePatient();
            break;
        case 5: return;
            break;

        default: WriteLine("invalid");
    }
}

void Console::doctors()
{
   WriteLine("----Doctors-----         ");
   WriteLine("[1] Add new doctor       ");
   WriteLine("[2] View all             ");
   WriteLine("[3] Search               ");
   WriteLine("\n[4] ->Back to Main Menu");

   int choice = 0;
   std::cin >> choice;

   Clear();

   switch (choice)
    {
        case 1: addDoctor();
            break;
        case 2: viewDoctors();
            break;
        case 3: viewDoctor();
            break;
        case 4: return;
            break;

        default: WriteLine("invalid");
    }
}

void Console::inventory()
{
   WriteLine("----Inventory-----");
   WriteLine("[1] Add new medicine");
   WriteLine("[2] View all");
   WriteLine("[3] Update Item Stock");
   WriteLine("\n[4] ->Back to Main Menu");

   int choice;
   cin.ignore();
   getNumber("input choice > ", choice, MinMax(1,4));

   Clear();

   switch (choice)
    {
        case 1: addItem();
            break;
        case 2: viewItems();
            break;
        case 3: updateStock();
            break;
        case 4: return;
            break;

        default: WriteLine("invalid");
    }
}

void Console::medical()
{
   WriteLine("----Medical Records-----");
   WriteLine("[1] Add new record");
   WriteLine("[2] View by department");
   WriteLine("\n[3] ->Back to Main Menu");

   int choice = 0;
   getNumber("Enter choice: ", choice, MinMax(1,3));

   Clear();

   switch (choice)
   {
        case 1: addRecord(); 
            break;
        case 2: viewDepartmentRecords(); 
            break;
        case 3: return;
   }
}

void Console::addPatient()
{
    WriteLine("[Adding New Patient]\n");
    cin.ignore();
    string insurer = "";

    Patient patient;
    patient.lastName = Prompt("Enter last name: ");
    patient.firstName = Prompt("Enter first name: ");

    int month, day, year;
    int heartrate, painlevel;
    float temp;

    getNumber("Enter a month of birth (number)", month,   MinMax(1,12));
    getNumber("Enter a day of birth   (number)",   day,   MinMax(1, 31));
    getNumber("Enter a year of birth  (number)",  year,   MinMax(1700, 2099));

    patient.dob = formatDate(month, day, year);

    patient.address = Prompt("Enter address: ");
    getNumber("Enter heartrate: ", heartrate, MinMax(1,-1));
    getNumber("Enter painlevel: ", painlevel, MinMax(1,-1));
    cout << "Enter temp: "; cin >> temp;
    patient.setVitals(heartrate, painlevel, temp);

    cin.ignore();
    insurer = Prompt("Enter insurace company (leave blank if none): ");

    patient.setInsurer(insurer);

    patientsdb.add(patient);
    patientsdb.save();
}

void Console::updatePatient()
{
    int patientId;
    getNumber("Enter patient id: (number): ", patientId, MinMax(1,999));
    auto it = patientsdb.find(patientId);

    if (it == patientsdb.all().end()) {
        WriteLine("Patient does not exist");
        return;
    }

    int heartrate, painlevel;
    float temp;

    getNumber("Enter heartrate: ", heartrate, MinMax(1,-1));
    getNumber("Enter painlevel: ", painlevel, MinMax(1,-1));
    cout << "Enter temp: "; cin >> temp;
    it->setVitals(heartrate, painlevel, temp);

    cout << "\n[Vitals updated]" << endl;
    patientsdb.save();
}

void Console::addDoctor()
{
    if (!isAdmin()) return;

    WriteLine("[Adding New Doctor]\n");
    cin.ignore();

    Doctor doctor;
    string passkey;

    doctor.lastName  = Prompt("Enter last name: ");
    doctor.firstName = Prompt("Enter first name: ");
    passkey = Prompt("Set Doctor Passkey: ");

    WriteLine("Available departments: ");
    for (auto& department : departmentsdb.all())
        cout << "[" << department.getID() << "]" << department.dptname << endl;

    getNumber("Enter department id: ", doctor.departmentid, departmentsdb.all());

    doctorsdb.add(doctor);
    WriteLine("\n[Doctor successfully added].");
    doctorsdb.save();
}

void Console::addRecord()
{
    WriteLine("[Adding New Patient Medical Record]\n");
    Record record;
    cin.ignore();

    if (patientsdb.all().empty()) {
        cout << "Unavailable";
        return;
    }
    
    getNumber("Enter patient id: ",    record.patientID,    patientsdb.all());

    WriteLine("Available departments: ");
    for (auto& department : departmentsdb.all())
        cout << department.getID() << ": " << department.dptname << endl;

    getNumber("Enter department id: ", record.departmentid, departmentsdb.all());

    record.date         = Date().getDate(TODAY);
    record.diagnosis    = Prompt("Enter diagnosis: ");

    prettyTable({"id", "name", "quantity", "cost"}, inventorydb.all());

    WriteLine("----------Prescriptions---------");
    cout << "How many items to add? ";
    int count;
    cin >> count;
    WriteLine("--------------------------------");
    auto patient = patientsdb.find(record.patientID);

    cin.ignore();
    double totalCost;
    for (int i = 0; i < count; i++)
    {
        int itemid, quantity;
        getNumber("Enter item id: ", itemid, inventorydb.all());
        auto item = inventorydb.find(itemid);
        getNumber("Enter quantity: ", quantity, MinMax(1, item->Quantity));
        record.prescriptions[item->Name] = quantity;
        item->Quantity -= quantity;
        totalCost += item->Cost * quantity;
        WriteLine("-------------------------");
    }
    WriteLine("-------------------------");
  
    record.treatment = Prompt("Enter Treatments: ");

    if (patient->hasInsurer())
    {
        Claim claim(record, totalCost, patient->getInsurer());
        claimsdb.add(claim);
    }
    else
    {
        patient->balance += totalCost;
    }
    recordsdb.add(record);

    recordsdb.save();
    inventorydb.save();
    patientsdb.save();
}

void Console::addItem()
{
    if (!isAdmin()) return;

    WriteLine("[Adding New Medicine Item]\n");
    cin.ignore();

    Item item;
    item.Name = Prompt("Enter Medicine name: ");

    getNumber("Enter medicine Quantiy:", item.Quantity, MinMax(1, -1));
    

	inventorydb.add(item);
    WriteLine("Medicine Item successfully added.");
    inventorydb.save();
}

void Console::viewDepartmentRecords()
{
    WriteLine("[Viewing Records by Department]\n");

    int deptid;
    cin.ignore();

    if (departmentsdb.all().empty()) {
        cout << "No departments";
        return;
    }
    getNumber("Enter department id: (number): ", deptid, departmentsdb.all());
    std::vector<Record> results = recordsdb.where(
        [&deptid](const Record& r) { return r.departmentid == deptid; });
    
    if (results.empty()) {
        WriteLine("No records found");
        return;
    }

    prettyTable({
        "id", 
        "patientid", 
        "departmentid", 
        "date",
        "diagnosis",
        "prescriptions",
        "treatments"},
        results);
    
    
}

void Console::viewPatients()
{
    //if (!isAdmin()) return;
    WriteLine("Viewing patients");

    WriteLine("[Boolinq test, has insurance, orderby balance]");
    auto results = patientsdb.query()
                        .where( [](const Patient& patient) {return patient.hasInsurer() == true;} )
                        .orderBy( [](const Patient& patient) { return patient.balance;} )
                        .toStdVector();

    prettyTable({"id",
                 "lastname",
                 "firstname",
                 "dob",
                 "address",
                 "balance"},
                results);
}

void Console::viewDoctors()
{
    if (!isAdmin()) return;
    WriteLine("[Viewing doctors]\n");
    prettyTable({"id",
                 "lastname",
                 "firstname",
                 "departmentid"},
                doctorsdb.all());
}

void Console::viewItems()
{
    if (!isAdmin()) return;

    WriteLine("[Viewing Medicine Items]\n");
    prettyTable({"ID", "Name", "Quantity", "Cost"}, inventorydb.all());
}

void Console::updateStock()
{
    int updateChoice;
    int ItemId, quantity;

    cout << "[Update Stock]\n" << endl;

    if (inventorydb.all().empty()) {
        cout << "No Items in Inventory";
        return;
    }

    cin.ignore();
    getNumber("Enter ItemID to update: ", ItemId, MinMax(1,999));
    
    auto item = inventorydb.find(ItemId);

    if (item == inventorydb.all().end())
    {
        cout << "Item does not exist.";
        return;
    }

    getNumber("Would you like to pull[1] or  add[2] stock: ", 
              updateChoice, MinMax(1,2) );

    if (updateChoice == 1 && !item->canPull()) {
        cout << "Item is not available";
        return;
    }

    string action = "Enter number of items to ";
    action  += updateChoice == 1 ? "pull: " : "add: ";

    int max = updateChoice == 1 ? item->Quantity : -1;
    getNumber(action, quantity, MinMax(1, max));

    switch (updateChoice)
    {
        case 1: item->pull(quantity); break;
        case 2: item->add(quantity);  break;
    }

    cout << "\n[Item succesfully updated]\n";
    cout << "Item [" + item->Name + "] new quantity: ";
    cout << to_string(item->Quantity) << endl;
    inventorydb.save();

}

void Console::findPatient()
{
    int patientId; cin.ignore();
    getNumber("Enter patient id: (number): ", patientId, MinMax(1,999));
    auto it = patientsdb.find(patientId);

    if (it == patientsdb.all().end()) {
        WriteLine("Patient does not exist");
        return;
    }
    const auto records = recordsdb.where(
        [&patientId](const Record& r) {return r.patientID == patientId;});

    const auto claims = claimsdb.where(
        [&patientId](const Claim& c) { return c.getPatientId() == patientId;});
    
    const auto appointments = appointmentsdb.where(
        [&patientId](const Appointment& a) { return a.getPatientID() == patientId;});

    Table table, info, records_t, vitals, claims_t, appointments_t;
    Table info_vitals;
    info.add_row({"Patient ID: ", to_string(it->getID())});
    info.add_row({"Last Name: ", it->lastName});
    info.add_row({"First Name: ", it->firstName});
    info.add_row({"Date of Birth: ", it->dob});
    info.add_row({"Balance: ", to_string(it->balance)});
    info.add_row({"Address: ", it->address});

    vitals.add_row({"Heartrate", to_string(it->heartrate)});
    vitals.add_row({"Pain level", to_string(it->painlevel)});
    vitals.add_row({"temperature", to_string(it->temp)});

    info_vitals.add_row({"Info", "Vitals"});
    info_vitals.add_row({info, vitals});

    records_t = makeTable(Record().row_headers(), records);
    claims_t = makeTable(Claim().row_headers(), claims);
    appointments_t = makeTable(Appointment().row_headers(), appointments);

    table.add_row({"Personal Info"});
    table.add_row({info_vitals});
    table.add_row({"Records"});
    table.add_row({records_t});
    table.add_row({"Appointments"});
    table.add_row({appointments_t});
    table.add_row({"Claims"});
    table.add_row({claims_t});

    cout << table << endl;
}

void Console::messages()
{
   WriteLine("----Messages[Doctor to Doctor]-----");
   WriteLine("[1] Send New Message              ");
   WriteLine("[2] View all                      ");
   WriteLine("[3] View my inbox                 ");
   WriteLine("\n[4] ->Back to Main Menu         ");

   int choice; cin.clear(); cin.ignore();
   getNumber("input choice > ", choice, MinMax(1,4));

   Clear();

   switch (choice)
    {
        case 1: addMessage();
            break;
        case 2: viewAllMessages();
            break;
        case 3: viewInbox();
            break;
        case 4: return;
            break;

        default: WriteLine("invalid");
    }
}

void Console::addMessage()
{
    cout << "[Compose New Message to Doctor]\n" << endl;
    int sender, receiver;
    std::vector<Doctor> all = doctorsdb.all();

    getNumber("From[Your Doctor id]: ", sender, all);
    getNumber("To[Receiver Doctor id]: ", receiver, all);

    std::string body;
    body = Prompt("Write Message: \n --> ");

    auto it = doctorsdb.find(sender);
    string passkey = Prompt("Enter doctor passkey to confirm: ");

    if (passkey != it->getPasskey())
    {
        cout << "[!Invalid passkey.]" << endl;
        return;
    }

    messagesdb.add({sender, receiver, body});
    cout << "[Message Successfully added]" << endl;
    messagesdb.save();
}

void Console::viewInbox()
{
    cout << "[View your messages]\n" << endl;

    int doctorid; cin.ignore();
    getNumber("Enter Doctor id: ", doctorid, MinMax(1,999));

    auto doctor = doctorsdb.find(doctorid);

    if (doctor == doctorsdb.all().end()) {
        WriteLine("Doctor does not exist");
        return;
    }

    string passkey = Prompt("Enter doctor passkey to confirm: ");
    
    if (passkey != doctor->getPasskey())
    {
        cout << "[!Invalid passkey.]" << endl;
        return;
    }

    const auto messages = messagesdb.where(
        [&doctorid](const Message& m) {return m.getReceiverID() == doctorid;});
    
    cout << "[Received Messages]\n\n";
    
    prettyTable({
        "id",
        "sender_id",
        "receiver_id",
        "body"
    }, messages, 1);

}

void Console::viewAllMessages()
{
    if (!isAdmin()) return;

    prettyTable({
        "id",
        "sender_id",
        "receiver_id",
        "body"
    }, messagesdb.all(), 1);
}

void Console::appointments()
{
   WriteLine("----------Appointments------------");
   WriteLine("[1] Make Appointment              ");
   WriteLine("[2] Cancel Appointment            ");
   WriteLine("\n[3] ->Back to Main Menu         ");

   int choice; cin.clear(); cin.ignore();
   getNumber("input choice > ", choice, MinMax(1,3));

   Clear();

   switch (choice)
    {
        case 1: addAppointment();
            break;
        case 2: cancelAppointment();
            break;
        case 3: return;
            break;

        default: WriteLine("invalid");
    }
}

void Console::addAppointment()
{
    cout << "[Add appointment (automatic doctor patient matching)]" << endl;

    int patientid, deptid;
    string subject, date;

    getNumber("Enter Patient ID: ", patientid, MinMax(1,999));
    
    auto patient = patientsdb.find(patientid);

    if (patient == patientsdb.all().end())
    {
        cout << "\n[Invalid patient id]";
        return;
    }

    WriteLine("Available departments: ");
    for (auto& department : departmentsdb.all())
        cout << department.getID() << ": " << department.dptname << endl;
    
    getNumber("Enter Department id: ", deptid, departmentsdb.all());

    std::vector<Doctor> doctors = doctorsdb.where(
        [&deptid](const Doctor& d) { return d.departmentid == deptid; });

    if (doctors.empty()) {
        cout << "No available doctors";
        return;
    }
    
    int min = doctors[0].numAppointments;
    Doctor selectedDoctor = doctors[0];

    for (auto doc : doctors) {
        if (doc.numAppointments < min)
            selectedDoctor = doc;
    }

    date = Date().getDate(TOMORROW);
    int hour = 0;

    auto doctor = doctorsdb.find(selectedDoctor.getID());

    while (true)
    {
        // try to book the appointment at current date
        hour = doctor->book_appointment(date);
        if (hour != -1)
            break;
        // current date is fully booked, check next day
        date = Date()
                .setDate(date)
                .increment()
                .getString();
    }
    
    cout << "---------------------------" << endl;
    cout << "[New appointment created]" << endl;

    cout << "Patient: " << patient->fullName()  << endl;
    cout << "Doctor: "  << doctor->fullName()   << endl;
    cout << "Date: "    << date                 << endl ;
    cout << "Time: "    << Date().getHour(hour) << endl ;

    cout << "What is the subject of the appointment? \n -->  ";
    subject = Prompt("Enter appointment subject: ");

    appointmentsdb.add({patientid, doctor->getID(), date, hour, subject});
    doctor->numAppointments += 1;
    
    cout << "\n[Appointment successfully created]" << endl;

    appointmentsdb.save();
    doctorsdb.save();

}

void Console::cancelAppointment()
{
    cout << "[Add appointment (automatic doctor patient matching)]" << endl;
    int appt_id;
    getNumber("Enter apopintment id: ", appt_id, MinMax(1,999));
    auto it = appointmentsdb.find(appt_id);

    if (it == appointmentsdb.all().end()) {
        cout << "appointment does not exist" << endl;
        return;
    }

    appointmentsdb.remove(appt_id);

    cout << "\n[Appointment successfully cancelled]" << endl;
    appointmentsdb.save();
}

void Console::billings()
{
    WriteLine("--------Billings---------");
    WriteLine("[1] Manage Claim         ");
    WriteLine("\n[2] ->Back to Main Menu");
    WriteLine("-------------------------");

    int choice;
    getNumber("input choice > ", choice, MinMax(1,2));

    Clear();
    
    switch (choice)
    {
        case 1 : manageClaim(); break;
        case 2: return;
    }
}

void Console::manageClaim()
{
    if (!isAdmin()) return;

    cout << "[Manage Claim]\n" << endl;

    if (claimsdb.all().empty()) {
        cout << "No claims available";
        return;
    }

    int claimId, option;
    getNumber("Enter claim id: ", claimId, MinMax(1,-1));

    auto claim = claimsdb.find(claimId);
    if (claim == claimsdb.all().end()) {
        cout << "Claim not found";
        return;
    }

    auto patient = patientsdb.find(claim->getPatientId());

    Table table = claim->toTable();
    cout << table << endl << endl;

    cout << "What would you like to do?\n";
    cout << "[1] nothing [2] Approve [3] Deny\n\n";

    getNumber("Enter option: ", option, MinMax(1,3));
    string msg;

    switch (option)
    {
        case 1: msg = "\nNothing to do"; break;

        case 2: claim->approve(); 
                msg = "\nClaim Approved";
                claimsdb.save();
                break;

        case 3: claim->deny();
                patient->owe(claim->getCost());
                msg = "\nClaim Denied";
                claimsdb.save();
                patientsdb.save();
                break;
    }

    cout << msg << endl;
}

void Console::viewDoctor()
{
    int doctorid; cin.ignore();
    getNumber("Enter doctor id: (number): ", doctorid, MinMax(1,-1));
    auto it = doctorsdb.find(doctorid);

    if (doctorsdb.all().empty() || it == doctorsdb.all().end()) {
        WriteLine("Patient does not exist");
        return;
    }

    Table table, info;
    table.format().font_align(FontAlign::center);
    table.add_row({"Viewing Doctor"});
    
    info.add_row({"full Name", it->fullName()});
    info.add_row({"Department id", to_string(it->getDeptID())});
    table.add_row({info});

    cout << table << endl;
}

void Console::WriteLine(const string& prompt, int spaces)
{
    cout << std::string(spaces, ' ') << prompt << "\n";
}

bool Console::isAdmin()
{
    cin.ignore();
    cout << "[This function requires admin access]" << endl;
    string key = Prompt("Enter passkey: ");

    if (!(key == "zxasqw12"))
    {
        cout << "\n[Invalid passkey. ]" << endl;
        return false;
    }
    Clear();
    return true;
}

void Console::Clear()
{
#if defined _WIN32
    system("cls");
    //clrscr(); // including header file : conio.h
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
    //std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
#elif defined (__APPLE__)
    system("clear");
#endif
}

void Console::onExit()
{
    patientsdb.save();
    inventorydb.save();
    departmentsdb.save();
    recordsdb.save();
    messagesdb.save();
    appointmentsdb.save();
    doctorsdb.save();
    claimsdb.save();
}