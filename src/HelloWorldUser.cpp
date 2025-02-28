#include "HelloWorldPubSubTypes.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "HelloWorldPublisher.hpp"
#include "HelloWorldSubscriber.hpp"

using namespace eprosima::fastdds::dds;

int main(int argc, char **argv){
    uint32_t samples = 10;
    HelloWorldPublisher mypub;
    HelloWorldSubscriber mysub;

    if(mypub.init() && mysub.init()){
        std::thread pub_td(&HelloWorldPublisher::run, &mypub, samples);
        std::thread sub_td(&HelloWorldSubscriber::run, &mysub, samples);

        pub_td.join();
        sub_td.join();
    }
    return 0;
}