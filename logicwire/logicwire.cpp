#include "logicwire.h"
#include <vector>
#include <functional>
#include <png++/png.hpp>

enum direction_t{
    RIGHT_DIRECTION,
    LEFT_DIRECTION,
    TOP_DIRECTION,
    BOTTOM_DIRECTION,
};

void LogicWire::loadCircuit(const char* image_name){
    // image into board
    png::image<png::rgb_pixel> image(image_name);
    string circuit = "";
    for (int a = 0; a < image.get_height(); a++) {
        for (int b = 0; b < image.get_width(); b++) {
            int color_hash = image[a][b].red +
                             image[a][b].green +
                             image[a][b].blue;
            char symbol = (color_hash > 0) ? '#' : ' '; // wire = #, other = 0
            circuit.push_back(symbol);
        }
    }
    board.load(circuit.c_str(), image.get_height());
    map.emptyBoard(image.get_height(), image.get_width());

    // giving an id for every inputs and outputs
    // addition, draw wire on board a place with pins
    uint input = 0;
    uint output = 0;
    for (int y = 0; y < board.getHeight(); y++) { // input wires
        if (board(0, y)) {
            inputs.push_back(Input(input));
            input++;
            map[y].addWire(0, 0); // "0" id for inputs and outputs
        }
        
        int last_x = board.getWidth() - 1;
        if (board(last_x, y)) {
            outputs.push_back(Output(output));
            output++;
            map[y].addWire(0, 0);
        }
    }
    
    // serching wires and bridges
    unsigned short int wire = 0;
    wires.push_back(false); // "0" wire. Always turn off. it doesn't exist on circuit
    for (int y = 1; y < board.getHeight() - 1; y++) {
        for (int x = 1; x < board.getWidth() - 1; x++) {
            if (board(x, y) && !map(x, y)) {
                wire++;
                wires.push_back(false);
                // prepare functions
                function<void(int, int)> walker; // <-- serching all wires
                                    // walker(x, y)
                function<void(int, int, int)> bridge; // checking for bridges
                                    // bridge(x, y, source_direction)
                
                bridge = [&](int x, int y, int source) {
                    long mask = 0x000000000;
                    if(board(x-1, y-1)) mask |= 0x100000000;
                    if(board(x  , y-1)) mask |= 0x010000000;
                    if(board(x+1, y-1)) mask |= 0x001000000;
                    if(board(x-1, y  )) mask |= 0x000100000;
                    if(board(x  , y  )) mask |= 0x000010000;
                    if(board(x+1, y  )) mask |= 0x000001000;
                    if(board(x-1, y+1)) mask |= 0x000000100;
                    if(board(x  , y+1)) mask |= 0x000000010;
                    if(board(x+1, y+1)) mask |= 0x000000001;

                    // bridge types
                    if (mask == 0x010101010) {
                        switch (source) {
                            case RIGHT_DIRECTION:
                                walker(x+1, y); break;
                            case LEFT_DIRECTION:
                                walker(x-1, y); break;
                            case TOP_DIRECTION:
                                walker(x, y+1); break;
                            case BOTTOM_DIRECTION:
                                walker(x, y-1); break;
                            default: break;
                        }   
                    }
                };
                walker = [&](int x, int y) {
                    if (map(x,y))
                        return;
                    map[y].addWire(x, wire); //for debug map.print() try addWire(wire + 48)
                    uint color_hash = image[y][x].red +
                                      image[y][x].green +
                                      image[y][x].blue;
                    if (color_hash == 255 * 3)
                        powerTheWire(wire);
                    
                    // check bridge
                    (board(x+1, y)) ? walker(x+1, y) : bridge(x+1, y, RIGHT_DIRECTION );
                    (board(x-1, y)) ? walker(x-1, y) : bridge(x-1, y, LEFT_DIRECTION  );
                    (board(x, y+1)) ? walker(x, y+1) : bridge(x, y+1, TOP_DIRECTION   );
                    (board(x, y-1)) ? walker(x, y-1) : bridge(x, y-1, BOTTOM_DIRECTION);
                    
                };
                walker(x, y);
            }
        }
    
    }

    // connecting inputs with wire
    for (int row = 0; row < board.getHeight(); row++) {
        int in = 0;
        if (map(0, row)) {
            inputs[in].setLocalConnection(map(1, row));
            in++;
        }

        int last_x = board.getWidth() - 1;
        int out;
        if (map(last_x, row)) {
            outputs[out].setLocalConnection(map(last_x - 1, row));
            out++;
        }
    }
    
    // searching gates
    int gate = 0;
    for (int y = 2; y < board.getHeight() - 2; y++) {
        for (int x = 2; x < board.getWidth() - 2; x++) {
            long mask = 0x000000000;
            if(board(x-1, y-1)) mask |= 0x100000000;
            if(board(x  , y-1)) mask |= 0x010000000;
            if(board(x+1, y-1)) mask |= 0x001000000;
            if(board(x-1, y  )) mask |= 0x000100000;
            if(board(x  , y  )) mask |= 0x000010000;
            if(board(x+1, y  )) mask |= 0x000001000;
            if(board(x-1, y+1)) mask |= 0x000000100;
            if(board(x  , y+1)) mask |= 0x000000010;
            if(board(x+1, y+1)) mask |= 0x000000001;

            // gate types
            switch (mask) {
                // left-to-right
                case 0x110101110:
                    gate++;
                    gates.push_back(Gate(gate, map(x-1,y), map(x+1,y)));
                    break;
                // right-to-left
                case 0x011101011:
                    gate++;
                    gates.push_back(Gate(gate, map(x+1,y), map(x-1,y)));
                    break;
                // top-to-bottom
                case 0x111101010:
                    gate++;
                    gates.push_back(Gate(gate, map(x,y-1), map(x,y+1)));
                    break;
                // bottom-to-top
                case 0x010101111:
                    gate++;
                    gates.push_back(Gate(gate, map(x,y+1), map(x,y-1)));
                    break;
                default: break;
            }
        }
    }
    // cout << "gates count: " << gate << endl; // for debug
}

// ############# OTHER FUNCTIONS #############

void LogicWire::powerTheInput(int id, bool status){
    inputs[id].setPower(status);
}

void LogicWire::simulate(){
    vector<bool> new_states;
    // vector<bool> new_input_states;
    vector<bool> new_output_states;
    new_states.resize(wires.size());
    // new_input_states.resize(inputs.size());
    new_output_states.resize(outputs.size());

    // input push power
    for (uint a = 0; a < inputs.size(); a++) {
        if (inputs[a].checkPower())
            new_states[inputs[a].getLocalConnection()] = true;
    }

    // gate stop/push power
    for (uint a = 0; a < gates.size(); a++) {
        Gate& gate = gates[a];
        bool source_powered = wires[gate.source];
        if (!source_powered)
            new_states[gate.drain] = true;
    }

    // output get power
    for (uint a = 0; a < outputs.size(); a++) {
        if (wires[outputs[a].getLocalConnection()])
            new_output_states[a] = true;
    }
    
    // apply changes
    wires = new_states;
    for (uint a = 0; a < outputs.size(); a++) {
        outputs[a].setPower(new_output_states[a]);
    }
}

void LogicWire::powerTheWire(int id){
    this->wires.at(id) = true;
}

// in future i won't need it
void LogicWire::print(){
    const char* bright = "\033[32m";
    const char* normal = "\033[37m";
    const char* reset = "\033[0m";

    for (int y = 0; y < board.getHeight(); y++) {
        for (int x = 0; x < board.getWidth(); x++) {
            int wire = map(x, y);
            char v = (board(x, y));
            cout << (wires[wire] ? bright : normal)
                    << (v ? v : ' ');
        }
        cout << reset << endl;
    }
}