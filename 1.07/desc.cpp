#include "desc.h"
int parse_monster_desc()
{
    const char *home = getenv("HOME");
    char *filename;
    size_t len;
    string s;
    len = (strlen(home) + strlen(SAVE_DIR) + strlen("monster_desc.txt") +
           1 /* The NULL terminator */ +
           2 /* The slashes */);

    filename = (char *)malloc(len * sizeof(*filename));
    sprintf(filename, "%s/%s/", home, SAVE_DIR);
    strcat(filename, "monster_desc.txt");
    ifstream f(filename);
    //f>>s goes one word at a time
    //f.get() gets and deletes the character
    //f.peek() gets the character
    //getline(f,s) gets the whole line excluding the \n

    //10 is \n
    getline(f, s);
    //
    if (s != "RLG327 MONSTER DESCRIPTION 1")
    {
        cerr << "Invalid file header" << endl;
    }
    else
    {

        while (f.peek() != -1)
        { //finds next monster
            getline(f, s);
            if (s == "BEGIN MONSTER")
            {
                parse_monster(f, s);
            }
            else
            {
                //cerr << "invalid BEGIN MONSTER tag" << endl;
            }
        }
    }
    f.close();
    //-1 is something unsure what
    //find semantic if it is present see what comes next
    //and parse the file, if all things are present
    //i.e. the number of valid fields is 9 and
    //the monster has begining and end tag
    return 0;
}
int parse_monster(ifstream &f, string s)
{
    int sum = 0;
    bool name, symb, color, speed, abil, hp, dam, description, rrty;
    name = symb = color = speed = abil = hp = dam = description = rrty = false;
    desc d;
    while (f >> s && s != "END" && sum < 10)
    {

        if (s == "NAME" && !name)
        {
            name = true;
            sum += parse_name(f, s, d);
        }
        else if (s == "SYMB" && !symb)
        {
            symb = true;
            sum += parse_syb(f, s, d);
        }
        else if (s == "COLOR" && !color)
        {
            color = true;
            sum += parse_color(f, s, d);
        }
        else if (s == "DESC" && !description)
        {
            description = true;
            sum += parse_desc(f, s, d);
        }
        else if (s == "SPEED" && !speed)
        {
            speed = true;
            sum += parse_speed(f, s, d);
        }
        else if (s == "DAM" && !dam)
        {
            dam = true;
            sum += parse_dam(f, s, d);
        }
        else if (s == "HP" && !hp)
        {
            hp = true;
            sum += parse_hp(f, s, d);
        }
        else if (s == "ABIL" && !abil)
        {
            abil = true;
            sum += parse_abil(f, s, d);
        }
        else if (s == "RRTY" && !rrty)
        {
            rrty  =true;
            sum += parse_rrty(f, s, d);
        }
        else
        {
            return 1;
        }
    }
    f.get();
    if (sum == 9)
    {
        print_desc(d);
    }
    return 0;
}
int parse_name(ifstream &f, string s, desc &d)
{
    f.get();
    getline(f, s);
    if(s == "\n"){
        return 0;
    }
    d.name = s;
    return 1;
}
int parse_syb(ifstream &f, string s, desc &d)
{
    f.get();
    if(f.peek() == '\n'){
        return 0;
    };
    d.symb = f.get();
    if(f.peek() != '\n'){
        return 0;
    }
    return 1;
}
int parse_color(ifstream &f, string s, desc &d)
{
    //make sure color is valid color
    f.get();
    while(f.peek() != '\n'){
        f>>s;
        int i = 0;
        bool found = false;
        for(; i<(int)pos_colors.size() && !found;i++){
            if(s == pos_colors.at(i)){
                found = true;
            }
        }
        if(!found){
            return 0;
        }
        d.color += (s+= " ");
    }
    
    return 1;
}
int parse_desc(ifstream &f, string s, desc &d)
{

    f.get();
    while (getline(f, s) && s != ".")
    {
        if (s.length() < 78)
        {
            d.description += (s += '\n');
            s = "";
        }
        else
        {
            return 0;
        }
    }
    d.description.erase(d.description.end() - 1);
    return 1;
}
int parse_speed(ifstream &f, string s, desc &d)
{
    f >> s;
    d.speed.base = stoi(s.substr(0, s.find("+")));
    d.speed.number = stoi(s.substr(s.find("+"), s.find("d")));
    d.speed.sides = stoi(s.substr(s.find("d") + 1, s.length()));
    return 1;
}
int parse_dam(ifstream &f, string s, desc &d)
{
    f >> s;
    d.dam.base = stoi(s.substr(0, s.find("+")));
    d.dam.number = stoi(s.substr(s.find("+"), s.find("d")));
    d.dam.sides = stoi(s.substr(s.find("d") + 1, s.length()));
    return 1;
}
int parse_hp(ifstream &f, string s, desc &d)
{
    f >> s;
    d.hp.base = stoi(s.substr(0, s.find("+")));
    d.hp.number = stoi(s.substr(s.find("+"), s.find("d")));
    d.hp.sides = stoi(s.substr(s.find("d") + 1, s.length()));
    return 1;
}
int parse_abil(ifstream &f, string s, desc &d)
{
    f.get();
    while(f.peek() != '\n'){
        f>>s;
        int i = 0;
        bool found = false;
        for(; i<(int)pos_abil.size() && !found;i++){
            if(s == pos_abil.at(i)){
                found = true;
            }
        }
        if(!found){
            return 0;
        }
        d.abil += (s+= " ");
    }
    
    return 1;
}
int parse_rrty(ifstream &f, string s, desc &d)
{
    getline(f, s);
    int tmp = stoi(s);
    if(tmp<1 || tmp > 100){
        return 0;
    }
    d.rrty = tmp;
    return 1;
}
void print_desc(desc &d)
{
    cout << d.name << endl;
    cout << d.description << endl;
    cout << d.color << endl;
    cout << d.speed.base << "+" << d.speed.number << "d" << d.speed.sides << endl;
    cout << d.abil << endl;
    cout << d.hp.base << "+" << d.hp.number << "d" << d.hp.sides << endl;
    cout << d.dam.base << "+" << d.dam.number << "d" << d.dam.sides << endl;
    cout << d.symb << endl;
    cout << d.rrty << endl;
    cout << endl;
}
