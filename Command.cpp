#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Command {
public:
    virtual ~Command() = default;
    virtual bool execute() = 0;
    virtual void undo() = 0;
};

class Texting {
    std::string text;
public:
    Texting(std::string txt) : text(txt) {}
    void print() { std::cout << text << std::endl; }

    bool insert(int pos, std::string txt) {
        if (pos < 0 || pos > (int)text.length()) return false;
        text.insert(pos, txt);
        return true;
    }

    bool erase(int pos, int len, std::string& out) {
        if (pos < 0 || len < 0 || pos + len > (int)text.length()) return false;
        out = text.substr(pos, len);
        text.erase(pos, len);
        return true;
    }

    bool replace(int pos, std::string c, std::string& out) {
        if (pos < 0 || pos >= (int)text.length() || c.length() != 1) return false;
        out = text.substr(pos, 1);
        text.replace(pos, 1, c);
        return true;
    }
};

class InsertCommand : public Command {
    Texting& texting;
    int pos;
    std::string txt;
public:
    InsertCommand(Texting& t, int p, std::string tx) : texting(t), pos(p), txt(tx) {}
    bool execute() override { return texting.insert(pos, txt); }
    void undo() override {
        std::string dummy;
        texting.erase(pos, txt.length(), dummy);
    }
};

class DeleteCommand : public Command {
    Texting& texting;
    int pos;
    int len;
    std::string txt;
public:
    DeleteCommand(Texting& t, int p, int l) : texting(t), pos(p), len(l) {}
    bool execute() override { return texting.erase(pos, len, txt); }
    void undo() override { texting.insert(pos, txt); }
};

class ReplaceCommand : public Command {
    Texting& texting;
    int pos;
    std::string c;
    std::string b;
public:
    ReplaceCommand(Texting& t, int p, std::string nc) : texting(t), pos(p), c(nc) {}
    bool execute() override { return texting.replace(pos, c, b); }
    void undo() override {
        std::string dummy;
        texting.replace(pos, b, dummy);
    }
};

class Remote {
    std::vector<std::shared_ptr<Command>> undoHistory;
    std::vector<std::shared_ptr<Command>> redoHistory;
public:
    void press(std::shared_ptr<Command> cmd) {
        if (cmd->execute()) {
            undoHistory.push_back(cmd);
            redoHistory.clear();
        }
    }
    void undo() {
        if (!undoHistory.empty()) {
            auto cmd = undoHistory.back();
            undoHistory.pop_back();
            cmd->undo();
            redoHistory.push_back(cmd);
        }
    }
    void redo() {
        if (!redoHistory.empty()) {
            auto cmd = redoHistory.back();
            redoHistory.pop_back();
            cmd->execute();
            undoHistory.push_back(cmd);
        }
    }
};

int main(int argc, char const *argv[]) {
    Texting texting(""); 
    Remote remote;

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "exit") break;

        if (cmd == "print") {
            texting.print();
            continue;
        }
        if (cmd == "undo") { 
            remote.undo();
            continue; 
        }
        if (cmd == "redo") { 
            remote.redo();
            continue; 
        }

        std::shared_ptr<Command> newCmd = nullptr;

        if (cmd == "insert") {
            int pos;
            std::string text;
            if (std::cin >> pos >> text) newCmd = std::make_shared<InsertCommand>(texting, pos, text);
        } else if (cmd == "delete") {
            int pos, len;
            if (std::cin >> pos >> len) newCmd = std::make_shared<DeleteCommand>(texting, pos, len);
        } else if (cmd == "replace") {
            int pos;
            std::string c;
            if (std::cin >> pos >> c) newCmd = std::make_shared<ReplaceCommand>(texting, pos, c);
        }

        if (newCmd) remote.press(newCmd);
    }
    return 0;
}