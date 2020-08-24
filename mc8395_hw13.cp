/* mc8395_hw13
 * Notes to the User/Reviewer:
 * (1) My code compiles and runs properly in CLION and in linux command line. I have also tested on vscode.
 * (2) I have also added certain keywords suggested by ClION, such at "static" since, after researching them, they applied.
 * We have not officially learned some of these keywords, but I was told by a couple of TA's that we can now include
 * more advanced C++ concepts in our programs, and defer to our IDE's formatting suggestions.
 * (3) I've left in various debugging statements, which can be uncommented to show details on how the simulation functions.
 * (4) Finally, I've occasionally had an issue with pressing enter: sometimes it doesn't run until you hit it again.
 * Then it prints, correctly, twice. So if the simulation hangs on enter, just press it again! :) Thanks!
*/
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;
const int LEFT = -1;
const int RIGHT = 1;
const int UP = -WIDTH;
const int DOWN = WIDTH;
const int MAX = WIDTH * HEIGHT - 1;
const int MIN = 0;
const int POSITION_ARRAY[] = {LEFT,RIGHT,DOWN,UP};
//function to generate random positions for bugs. Does not need access to private member variables
void getRandomPositions(vector<int>& arrayOfPositions, vector<int>& arrayToCheck, int numberOfPositions);
class Organism {
public:
    //constructor
    Organism(): hasMoved(false), turnLastBred(0), position(0), organismSymbol('-'){}
    //destructor
    virtual ~Organism()= default;
    //pure virtual function to give all derived classes the move function
    virtual void move(Organism **&array, int simulationCount, int& organismCount) = 0;
    //breed function that is shared
    void breed(Organism**& array, int simulationCount, int& CreatureCount, char character);
    //setters
    void setHasMoved(bool moved){hasMoved = moved;};
    void setTurnLastBred(int simulationCount){turnLastBred = simulationCount;}
    void setOrganismRep(char character){organismSymbol = character;};
    void setPosition(int pos, Organism**& array) {array[pos] = this; position = pos;}
    //only used for doodle
    virtual void setTurnLastAte(int simulationCount){}
    //getters
    bool getHasMoved(){return hasMoved;}
    int getLastBreedStep(){return turnLastBred;}
    int getPosition(){return position;};
    char getOrganismRep(){return organismSymbol;}
    //only used for doodle
    virtual int getTurnLastAte(){return 0;}
    //high-level functions that require access to private info
    static bool isLegalMove(int moveDirection, int moveChoice, int currentPosition);
    static void getOpenMoves(Organism**& array, int currentPosition,vector<int>& openPositions);
private:
    bool hasMoved;
    //variable to keep track of turn last bred
    int turnLastBred;
    //what index it's at in the array
    int position;
    //the visual representation of the organism on the board
    char organismSymbol;
};
class Ant: public Organism {
public:
    Ant();
    ~Ant() override = default;;
    void move(Organism **&array,int __unused simulationCount, int& AntCount) override;
    //breed is inherited
};
class Doodlebug: public Organism{
public:
    explicit Doodlebug(int simulationCount);
    ~Doodlebug() override = default;
    void setTurnLastAte(int simulationCount) override;
    int getTurnLastAte() override{return turnLastAte;}
    void move(Organism **&array, int simulationCount, int& AntCount) override;
    static void findAntNearby(Organism**& array, int currentPosition, vector<int>& nearbyAnts);
    //breed is inherited from parent class
    //starve is handled in-line
private:
    int turnLastAte;
};
int main() {
    srand(time(nullptr));
    int simulationCount = 0;
    int AntCount = 0;
    int DoodlebugCount = 0;
    vector<int> randomDoodleBugs;
    vector<int> randomAnts;
    auto** organismArray = new Organism*[MAX+1];
    getRandomPositions(randomDoodleBugs,randomDoodleBugs,5);
    getRandomPositions(randomAnts,randomDoodleBugs,100);
    for(int randomDoodleBug : randomDoodleBugs){
        auto* doodle = new Doodlebug(simulationCount);
        doodle->setPosition(randomDoodleBug, organismArray);
        DoodlebugCount++;
    }
    for (int randomAnt : randomAnts){
        Ant *ant = new Ant;
        ant->setPosition(randomAnt, organismArray);
        AntCount++;
    }
    cout<<"Starting simulation"<<endl;
    for(int i = 0; i < WIDTH * HEIGHT; i++){
        if(i % WIDTH == 0 && i != 0)
            cout<<endl;
        if(organismArray[i] == nullptr)
            cout<<'-'<<"\t";
        else
            cout<<organismArray[i]->getOrganismRep()<<"\t";
    }
    cout<<"Number of Ants: "<<AntCount<<", Doodles: "<<DoodlebugCount<<", Time Steps: "<<simulationCount<<"\n"<<endl;
    cout<<endl<<"if exit type 1 otherwise press enter"<<"\n";
    char temp = '\n';
    while (temp == '\n'){
    //below commented out info is used to run simulation automatically
    //int turnCounter = 0;
    //while(turnCounter < 1000){
        cin.get(temp);
        //move the doodlebugs
        simulationCount++;
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (organismArray[i] != nullptr && organismArray[i]->getOrganismRep() == 'X'
            && !organismArray[i]->getHasMoved()) {
                    organismArray[i]->setHasMoved(true);
                    //includes an eat function
                    organismArray[i]->move(organismArray, simulationCount, AntCount);
            }
        }
        //move the ants
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (organismArray[i] != nullptr && organismArray[i]->getOrganismRep() == 'o'
            && !organismArray[i]->getHasMoved()) {
                    organismArray[i]->setHasMoved(true);
                    organismArray[i]->move(organismArray, simulationCount, AntCount);
            }
        }
        //breed the doodlebugs
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (organismArray[i] != nullptr && organismArray[i]->getOrganismRep() == 'X') {
                //if this particular bug has been alive 8 or more sessions
                    if (simulationCount - organismArray[i]->getLastBreedStep() == 8) {
                       // cout << "this dood" << organismArray[i]->getPosition() << " breeds in this many turns: " << organismArray[i]->getLastBreedStep() << endl;
                        organismArray[i]->breed(organismArray, simulationCount, DoodlebugCount, organismArray[i]->getOrganismRep());

                    }
            }
        }
        //breed the ants
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (organismArray[i] != nullptr) {
                if (organismArray[i]->getOrganismRep() == 'o') {
                    //if they've been alive for 3 turns, they breed
                    if (simulationCount - organismArray[i]->getLastBreedStep() == 3) {
                       // cout << "this ant" << organismArray[i]->getPosition() << " breeds in this many turns: "
                       //      << organismArray[i]->getLastBreedStep() << endl;
                        organismArray[i]->breed(organismArray, simulationCount, AntCount,
                                                organismArray[i]->getOrganismRep());
                    }
                }
            }
        }
        //starve the doodlebugs
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (organismArray[i] != nullptr && organismArray[i]->getOrganismRep() == 'X') {
                if (simulationCount - organismArray[i]->getTurnLastAte() == 3){
                    //cout<<"starving doodle: "<<organismArray[i]->getPosition()<<" who didn't eat in ";
                    //cout<<simulationCount - organismArray[i]->getTurnLastAte()<<"turns"<<endl;

                    /* deleting the doodle "starves" it. I opted to not use a function for this since I didn't want to
                    * write `delete this` inside the function. That is generally considered dangerous/bad practice. */
                    delete organismArray[i];
                    organismArray[i] = nullptr;
                    DoodlebugCount--;
                }
            }
        }
        cout << endl;
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (i % WIDTH == 0 && i != 0)
                cout << endl;
            if (organismArray[i] != nullptr) {
                cout << organismArray[i]->getOrganismRep() << "\t";
                //cleanup
                organismArray[i]->setHasMoved(false);
            }else
                cout<<'-'<<"\t";
        }
        cout<<"Number of Ants: "<<AntCount<<" Doodles: "<<DoodlebugCount<<" Time Steps: "<<simulationCount<<"\n"<<endl;
        //turn counter used to run simulation automatically
        //turnCounter++;
    }
    delete[] organismArray;
return 0;
}

//parent functions and shared functions
bool Organism::isLegalMove(int moveDirection, int moveChoice, int currentPosition){
    bool legal = true;
    legal = legal && (moveChoice <= MAX && moveChoice >= MIN);
    if(moveDirection == LEFT && currentPosition % WIDTH == 0)
        legal = false;
    if(moveDirection == RIGHT && (currentPosition % WIDTH == (WIDTH - 1)))
        legal = false;
    return legal;
}
void Organism::getOpenMoves(Organism**& array, int currentPosition,vector<int>& openPositions){
    for(int i : POSITION_ARRAY) {
        int index = i + currentPosition;
        if (array[index]->isLegalMove(i, index, currentPosition) &&
            (array[index] == nullptr))
            openPositions.push_back(index);
    }
}
void Organism::breed(Organism**& array, int simulationCount, int& CreatureCount, char character) {
    character = this->getOrganismRep();
    int currentPosition = this->getPosition();
    this->setTurnLastBred(simulationCount);
    int moveChoice = 0;
    vector<int> openMoves;
    getOpenMoves(array, currentPosition, openMoves);
    if(!openMoves.empty()) {
        moveChoice = openMoves[rand() % openMoves.size()];
        //create an appropriate bug
        if(character == 'o') {
            Ant *ant = new Ant;
            //put it in the new position
            ant->setPosition(moveChoice, array);
            ant->setTurnLastBred(simulationCount);
            CreatureCount++;
            //cout<<"new Ant count: "<<CreatureCount<<endl;
        }
        if(character == 'X'){
            auto* doodlebug = new Doodlebug(simulationCount);
            doodlebug->setPosition(moveChoice, array);
            doodlebug->setTurnLastBred(simulationCount);
            CreatureCount++;
            //cout<<"new doodle count: "<<CreatureCount<<endl;
        }

    }
}
//ANT FUNCTIONS
Ant::Ant() {
    setOrganismRep('o');
}
//CLION suggested I add the __unused modifier here, so I have.
void Ant::move(Organism **&array, int __unused simulationCount, int& AntCount) {
    bool legal;
    int currentPosition = this->getPosition();
    int moveChoiceIndex = rand() % 4;
    int moveDirection = POSITION_ARRAY[moveChoiceIndex];
    int moveChoice = currentPosition + moveDirection;
    //determine if move is legal
    legal = isLegalMove(moveDirection, moveChoice, currentPosition);
    //if it's legal but it's not null, then legal = false
    if(legal && array[moveChoice] != nullptr){
        legal = false;
    }
    if (legal) {
        this->setPosition(moveChoice,array);
        array[currentPosition] = nullptr;
    }
}
//DOODLEBUG FUNCTIONS
Doodlebug::Doodlebug(int simulationCount){
    setOrganismRep('X');
    turnLastAte = simulationCount;
}
void Doodlebug::findAntNearby(Organism**& array, int currentPosition, vector<int>& nearbyAnts){
    for(int i : POSITION_ARRAY){
        int availablePosition = i + currentPosition;
        bool legal = isLegalMove(i, availablePosition, currentPosition);
        //legal move, position isn't null, and it has an ant
        if (legal && array[availablePosition] != nullptr && array[availablePosition]->getOrganismRep() == 'o'){
            nearbyAnts.push_back(availablePosition);
        }
    }
}
void Doodlebug::move(Organism**& array, int simulationCount, int& AntCount){
    //if possible, eat an ant
    vector<int> nearbyAnts;
    this->findAntNearby(array, this->getPosition(), nearbyAnts);
    if(!nearbyAnts.empty()){
        int moveChoice = nearbyAnts[rand() % nearbyAnts.size()];
        int oldPos = this->getPosition();
        //delete the ant, aka eat it
        delete array[moveChoice];
        array[moveChoice] = nullptr;
        //move the doodlebug to the ant's old position
        this->setPosition(moveChoice, array);
        //make the old position no longer point to the doodlebug that has moved
        array[oldPos] = nullptr;
        //log the time the doodle ate
        this->setTurnLastAte(simulationCount);
        AntCount--;
        //cout<<"Count after dying: "<<AntCount<<endl;
    }else{
        //if it cannot eat an ant then it moves, according to the same functionality as an ant
        bool legal;
        int currentPosition = this->getPosition();
        int moveChoiceIndex = rand() % 4;
        int moveDirection = POSITION_ARRAY[moveChoiceIndex];
        int moveChoice = currentPosition + moveDirection;
        //determine if move is legal
        legal = isLegalMove(moveDirection, moveChoice, currentPosition);
        //determine if the move is in an empty slot
        if(legal && array[moveChoice] != nullptr){
            legal = false;
        }
        if (legal) {
            this->setPosition(moveChoice,array);
            array[currentPosition] = nullptr;
        }

    }
}
void Doodlebug::setTurnLastAte(int simulationCount) {
    turnLastAte = simulationCount;
}

//global functions
void getRandomPositions(vector<int>& arrayOfPositions, vector<int>& arrayToCheck, int numberOfPositions) {
    while (arrayOfPositions.size() != numberOfPositions) {
        int randomNum = rand() % (MAX+1);
        //this block of code makes sure the random number is distinct
        bool hasBeenUsed = false;
        for(int i : arrayToCheck) {
            if(randomNum == i)
                hasBeenUsed = true;
            //cout<<"rn"<<randomNum<<","<<i<< " ";
        }
        for(int i : arrayOfPositions) {
            if(randomNum == i)
                hasBeenUsed = true;
            //cout<<"rn"<<randomNum<<","<<i<< " ";
        }
       if(!hasBeenUsed){
           arrayOfPositions.push_back(randomNum);
        }
    }
}