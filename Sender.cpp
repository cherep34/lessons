#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>

class Sender {
public:
    virtual ~Sender() = default;
    virtual void send(std::string recipient, std::string message) = 0;
};


class EmailSender : public Sender {
public:
    void send(std::string r, std::string m) override {
        std::cout << "email: " << r << std::endl;
        std::cout << "Текст: " << m << std::endl;
    }
};


class SmsSender : public Sender {
public:
    void send(std::string r, std::string m) override {
        std::cout << "Номер: " << r << std::endl;
        std::cout << "Текст: " << m << std::endl;
    }
};


class TelegramSender : public Sender {
public:
    void send(std::string r, std::string m) override {
        std::cout << "Телеграм: " << r << std::endl;
        std::cout << "Текст: " << m << std::endl;
    }
};


class Notification {

public:
    Notification(std::shared_ptr<Sender> s, std::string r, std::string m, bool u)
        : sender(s), recipient(r), message(m), urgent(u) {}

    virtual ~Notification() = default;


    virtual void send() {
        sender->send(recipient, message);
    }

protected:

    std::shared_ptr<Sender> sender;
    std::string recipient;
    std::string message;
    bool urgent;

};


class RegularNotification : public Notification {
public:
    RegularNotification(std::shared_ptr<Sender> s, std::string r, std::string m)
        : Notification(s, r, m, false) {}
};


class UrgentNotification : public Notification {
public:
    UrgentNotification(std::shared_ptr<Sender> s, std::string r, std::string m)
        : Notification(s, r, m, true) {}

    void send() override {
        std::string urgentMessage = "ОЧЕНЬ СРОЧНО " + message;
        sender->send(recipient, urgentMessage);
    }
};

class Builder {
public:
    void setSender(std::shared_ptr<Sender> s){ this->sender = s; }
    void setRecipient(std::string r){ this->recipient = r; }
    void setMessage(std::string m){ this->message = m; }
    void setUrgent(bool u){ this->urgent = u; }

    std::shared_ptr<Notification> result(){
        if (urgent){
            return std::make_shared<UrgentNotification>(sender, recipient, message);
        }
        else {
            return std::make_shared<RegularNotification>(sender, recipient, message);
        }
    }

    void reset(){
        this->sender.reset();
        this->recipient.clear();
        this->message.clear();
        this->urgent = false;
    }
private:
    std::shared_ptr<Sender> sender;
    std::string recipient;
    std::string message;
    bool urgent;
};

class Director {
public:
    std::shared_ptr<Notification> Urgent(Builder& builder, std::shared_ptr<Sender> sender, std::string recipient, std::string message){
        builder.setSender(sender);
        builder.setRecipient(recipient);
        builder.setMessage(message);
        builder.setUrgent(true);
        std::shared_ptr<Notification> notify = builder.result();
        builder.reset();
        return notify;
    }
    std::shared_ptr<Notification> Regular(Builder& builder, std::shared_ptr<Sender> sender, std::string recipient, std::string message){
        builder.setSender(sender);
        builder.setRecipient(recipient);
        builder.setMessage(message);
        builder.setUrgent(false);
        std::shared_ptr<Notification> notify = builder.result();
        builder.reset();
        return notify;
    }
};

class Command{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
};

class SendNotification : public Command{
public:
    SendNotification(std::shared_ptr<Notification> n): notification(n) {}
    void execute() override {
        this->notification->send();
    }
private:
    std::shared_ptr<Notification> notification;
};

class Remote{
public:
    void execute(std::shared_ptr<Command> cmd){
        std::cout << "Команда выполнена" << std::endl;
        cmd->execute();
        this->history.push_back(cmd);
    }
    
    void countHistory(){
        std::cout << "Команд выполнено - " << this->history.size() << std::endl;
    }

private:
    std::vector<std::shared_ptr<Command>> history;
};

int main() {
    auto emailSender = std::make_shared<EmailSender>();
    auto smsSender = std::make_shared<SmsSender>();
    auto telegramSender = std::make_shared<TelegramSender>();

    Builder builder;
    Director director;

    Remote remote;
    
    remote.execute(std::make_shared<SendNotification>(director.Regular(builder, emailSender, "342@csu.ru", "Пары окончены")));
    remote.execute(std::make_shared<SendNotification>(director.Urgent(builder, smsSender, "+7900231311", "Победа")));
    remote.execute(std::make_shared<SendNotification>(director.Regular(builder, telegramSender, "Артём", "Когда подъедешь")));
    return 0;
}