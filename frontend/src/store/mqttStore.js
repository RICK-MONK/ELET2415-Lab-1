import { defineStore } from 'pinia'
import { ref } from 'vue'

export const useMqttStore = defineStore('mqtt', () => {

    // STATES 
    const mqtt          = ref(null);
    const host          = ref("www.yanacreations.com"); // Your Laptop IP
    const port          = ref(9001);          // Standard WebSocket Port
    const payload       = ref({"id":620012345,"timestamp": 1702566538,"number":0,"ledA":0,"ledB":0});
    const payloadTopic  = ref("");
    const subTopics     = ref({});

    // ACTIONS
    const onSuccess = () => {
        console.log(`MQTT: Connection request acknowledged`);
    }

    const onConnected = (reconnect, URI) => {
    console.log(`Connected to: ${URI} , Reconnect: ${reconnect}`);
    
    // AUTO-SUBSCRIBE UPON CONNECTION
    subscribe("620169874"); 

    if(reconnect){
        const topics = Object.keys(subTopics.value);
        topics.forEach((topic) => {
            subscribe(topic);
        });
    }
}

    const onConnectionLost = (response) => {
        if (response.errorCode !== 0) {
            console.log(`MQTT: Connection lost - ${response.errorMessage}`);
        }
    }

    const onFailure = (response) => {
        const hostName = response.invocationContext.host;   
        console.log(`MQTT: Connection to ${hostName} failed. \nError message : ${response.errorMessage}`);                  
    };
    
    const onMessageArrived = (response) => {
        try {
            payload.value       = JSON.parse(response.payloadString); 
            payloadTopic.value  = response.destinationName;
            console.log(`Topic : ${payloadTopic.value} \nPayload : ${response.payloadString}`);  
        } catch (error) {
            console.log(`onMessageArrived Error: ${error}`);
        }
    }

    const makeid = (length) => {
        var result           = '';
        var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
        var charactersLength = characters.length;
        for ( var i = 0; i < length; i++ ) {
            result += characters.charAt(Math.floor(Math.random() * charactersLength));
        }
        return "IOT_F_" + result;
    };

    // SUBSCRIBE FUNCTIONS
    const sub_onSuccess = (response) => {   
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Subscribed to - ${topic}`);  
        subTopics.value[topic] = "subscribed"; 
    }

    const sub_onFailure = (response) => {       
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Failed to subscribe to - ${topic} \nError message : ${response.errorMessage}`);  
    }

    const subscribe = (topic) => {
        try {
            if (mqtt.value && mqtt.value.isConnected()) {
                var subscribeOptions = { onSuccess: sub_onSuccess, onFailure: sub_onFailure, invocationContext: {"topic": topic} };
                mqtt.value.subscribe(topic, subscribeOptions);   
            } else {
                console.log("MQTT: Cannot subscribe, client not connected yet.");
            }
        } catch (error) {
            console.log(`MQTT: Unable to Subscribe ${error} `);
        }
    };

    // UNSUBSCRIBE FUNCTIONS
    const unSub_onSuccess = (response) => {    
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Unsubscribed from - ${topic}`);          
        delete subTopics.value[topic];
    }

    const unSub_onFailure = (response) => {   
        const topic = response.invocationContext.topic;  
        console.log(`MQTT: Failed to unsubscribe from - ${topic} \nError message : ${response.errorMessage}`);  
    }

    const unsubcribe = (topic) => {     
        var unsubscribeOptions = { onSuccess: unSub_onSuccess, onFailure: unSub_onFailure, invocationContext: {"topic": topic} };
        mqtt.value.unsubscribe(topic, unsubscribeOptions);         
    }
    
    const unsubcribeAll = () => {   
        const topics = Object.keys(subTopics.value);
        if(topics.length > 0) {
            topics.forEach((topic) => {
                var unsubscribeOptions = { onSuccess: unSub_onSuccess, onFailure: unSub_onFailure, invocationContext: {"topic": topic} };
                mqtt.value.unsubscribe(topic, unsubscribeOptions);
            });
        }   
        disconnect();       
    }

    // PUBLISH FUNCTION
    const publish = (topic, messageContent) => { 
        if (mqtt.value && mqtt.value.isConnected()) {
            const message = new Paho.MQTT.Message(messageContent);
            message.destinationName = topic;
            mqtt.value.send(message); 
            console.log("MQTT: Published to " + topic);
        } else {
            console.log("MQTT: Cannot publish, client is null or disconnected.");
        }
    };

    // DISCONNECT FUNCTION
    const disconnect = () => {  
        if (mqtt.value) mqtt.value.disconnect();                    
    }

    const connect = () => {
        var IDstring = makeid(12);
        console.log(`MQTT: Connecting to Server : ${host.value} Port : ${port.value}` );
        mqtt.value = new Paho.MQTT.Client( host.value, port.value, "/mqtt", IDstring );   
    
        var options = { 
            timeout: 3, 
            onSuccess: onSuccess, 
            onFailure: onFailure, 
            invocationContext: {"host": host.value, "port": port.value }, 
            useSSL: false, 
            reconnect: true, 
            uris:[`ws://${host.value}:${port.value}/mqtt`] 
        }; 
        
        mqtt.value.onConnectionLost = onConnectionLost;
        mqtt.value.onMessageArrived = onMessageArrived;
        mqtt.value.onConnected     = onConnected;
        mqtt.value.connect(options);    
    };

    return {  
        payload,
        payloadTopic,
        subscribe,
        unsubcribe,
        unsubcribeAll,
        publish,
        connect,
        disconnect,
    }
}, { persist: true });