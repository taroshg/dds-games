#ifndef DDS_GAME_CONTROLLER_HEADER
#define DDS_GAME_CONTROLLER_HEADER

#include "idl/GameMessagePubSubTypes.hpp"

#include <chrono>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <iostream>
#include <random>
#include <sstream>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;

class PubListener : public DataWriterListener
    {
    private:
        int matched_;
    public:
        PubListener() : matched_(0) {}
        ~PubListener() override {}

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        int matched() const {return matched_;}
};

class SubListener : public DataReaderListener
{
private:
    int matched_;
    unsigned long message_count_;
    std::string last_uid_;
    std::string uid_;
    GameMessage game_msg_;
public:
    // we are sticking to 2 player games!
    std::vector<std::string> participants_;

    SubListener() : 
    last_uid_(""),
    matched_(0), 
    message_count_(0), 
    participants_({}){}
    ~SubListener() override {}

    void set_uid(std::string uid){
        uid_ = uid;
    }

    void on_subscription_matched(
            DataReader*,
            const SubscriptionMatchedStatus& info) override
    {
        if (info.current_count_change == 1)
        {   
            matched_++;
            std::cout << "Subscriber matched. " << std::endl;
        }
        else if (info.current_count_change == -1)
        {
            matched_--;
            std::cout << "Subscriber unmatched." << std::endl;
        }
        else
        {
            std::cout << info.current_count_change
                    << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
        }
    }

    void on_data_available(DataReader* reader) override
    {
        SampleInfo info;
        GameMessage msg;
        if (reader->take_next_sample(&msg, &info) == eprosima::fastdds::dds::RETCODE_OK && info.valid_data)
        {
            // adds the participant uid to a list
            last_uid_ = msg.uid();

            // checks duplicates
            if (std::find(participants_.begin(), participants_.end(), last_uid_) == participants_.end())
                participants_.push_back(last_uid_);

            if (last_uid_ == uid_)
                return;
            
            game_msg_ = msg;
            message_count_++;
        }
    }

    int matched() const {return matched_;}
    unsigned long message_count() const {return message_count_;}
    std::vector<std::string>& participants() {return participants_;}
    std::string last_uid() const {return last_uid_;}
    GameMessage& game_msg() {return game_msg_;}
};

class DDSGameController{
private:
    DomainParticipant* participant_;
    Publisher* publisher_;
    Subscriber* subscriber_;
    Topic* topic_;
    DataWriter* writer_;
    DataReader* reader_;
    TypeSupport type_;

    PubListener pub_listener_;
    SubListener sub_listener_;

    unsigned long last_message_count_;
    std::string uid_;

    std::string generateUUID() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;
    
        uint64_t uuid = dis(gen);
    
        std::stringstream ss;
        ss << std::hex << uuid;
        return ss.str();
    }

public:
    DDSGameController() : participant_(nullptr)
    , publisher_(nullptr)
    , topic_(nullptr)
    , writer_(nullptr)
    , reader_(nullptr)
    , last_message_count_(0)
    , type_(new GameMessagePubSubType()) {
        uid_ = generateUUID();
        sub_listener_.set_uid(uid_);
    }

    virtual ~DDSGameController()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    bool init(){
        // creates a new participant
        DomainParticipantQos participantQos;
        participantQos.name("Participant_controller");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr) return false;
        type_.register_type(participant_);

        // create a topic
        topic_ = participant_->create_topic("GameTopic", "GameMessage", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr) return false;

        // creates publisher and writer
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
        if (publisher_ == nullptr) return false;
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &pub_listener_);
        if (writer_ == nullptr) return false;

        // creates subscriber and reader
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
        if (subscriber_ == nullptr) return false;
        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &sub_listener_);
        if (reader_ == nullptr) return false;

        return true;
    }

    std::string uid() const {return uid_;}

    bool publish(GameMessage* msg){
        // only publish if there is a listener
        if (pub_listener_.matched() > 0){
            msg->index(message_count() + 1);
            writer_->write(msg);
            return true;
        }
        return false;
    }

    int n_subscribers() const{
        return sub_listener_.matched();
    } 

    int n_publishers() const{
        return pub_listener_.matched();
    }

    std::string last_uid() const {return sub_listener_.last_uid();}

    unsigned long message_count() const {
        return sub_listener_.message_count();
    }

    std::vector<std::string>& participants() {return sub_listener_.participants();}

    GameMessage* read() {
        return &sub_listener_.game_msg();
    }
};

#endif