const Log = require('../models/Log');
const { getFirestore } = require('firebase-admin/firestore');
const { initializeApp, cert } = require('firebase-admin/app');
const serviceAccount = require('../config/service_key.json');
const axios = require('axios');

initializeApp({
  credential: cert(serviceAccount)
});
const db = getFirestore();

exports.getLogs = async (req, res) => {
    try {
        const logs = await Log.find().sort({ timestamp: -1 }).limit(20);
        res.status(200).json({success: true, data: logs});
    } catch (err) {
        res.status(400).json({success: false, message: err.message});
    }
}
exports.postLog = async (req, res) => {
    try{
        console.log(req.body);
        const log = await Log.create(req.body);
        await db.collection('logs').add(JSON.parse(JSON.stringify(log.toObject())));
        const webhookUrl = 'https://discord.com/api/webhooks/1310846461486825542/XIFm1JnAAwwBdIZlpvLttDOEnxtI961jADtggUJNH4D2OptqC3ChXx5UorvaFZSB3Des';
        const payload = {
            content: "<@299769980600320001>",
            embeds: [{
                title: "**Data Sent**",
                description: "Data was sent to the server.",
                fields: [
                    { name: "Temperature", value: `${log.temperature}°C`, inline: true },
                    { name: "Humidity", value: `${log.humidity}%`, inline: true },
                    { name: "Smoke Detected", value: log.smoke ? "Yes" : "No", inline: true },
                    { name: "Brightness", value: `${log.lux} lux`, inline: true },
                    { name: "People Count", value: `${log.people}`, inline: true }
                ],
                color: 65280,
                timestamp: new Date().toISOString()
            }]
        };

        if (log.smoke) {
            smoke_payload = {
                title: "**Notification**",
                description: "Smoke was detected.",
                color: 16711680,
                timestamp: new Date().toISOString()
            }
            payload.embeds.push(smoke_payload);
        }
        if (log.temperature <= 22 || log.temperature >= 27) {
            temperature_payload = {};
            if (log.temperature == 0){
                temperature_payload = {
                    title: "**Notification**",
                    description: "Cannot read data from temperature sensor.",
                    color: 16711680,
                    timestamp: new Date().toISOString()
                }
            }else{
                temperature_payload = {
                    title: "**Notification**",
                    description: "Temperature is out of range.",
                    color: 16711680,
                    fields: [
                        { name: "Temperature", value: `${log.temperature}°C`, inline: true },
                        { name: "Suitable Range", value: "22°C - 27°C", inline: true }
                    ],
                    timestamp: new Date().toISOString()
                }
            }
            payload.embeds.push(temperature_payload);
        }
        if (log.humidity <= 40 || log.humidity >= 60) {
            humidity_payload = {};
            if (log.humidity == 0){
                humidity_payload = {
                    title: "**Notification**",
                    description: "Cannot read data from humidity sensor.",
                    color: 16711680,
                    timestamp: new Date().toISOString()
                }
            }else{
                humidity_payload = {
                    title: "**Notification**",
                    description: "Humidity is out of range.",
                    color: 16711680,
                    fields: [
                        { name: "Humidity", value: `${log.humidity}%`, inline: true },
                        { name: "Suitable Range", value: "40% - 60%", inline: true }
                    ],
                    timestamp: new Date().toISOString()
                }
            }
            payload.embeds.push(humidity_payload);
        }
        if (log.lux <= 400 || log.lux >= 600) {
            brightness_payload = {};
            if (log.lux == 0){
                brightness_payload = {
                    title: "**Notification**",
                    description: "Cannot read data from brightness sensor.",
                    color: 16711680,
                    timestamp: new Date().toISOString()
                }
            }else{
                brightness_payload = {
                    title: "**Notification**",
                    description: "Brightness is out of range.",
                    color: 16711680,
                    fields: [
                        { name: "Brightness", value: `${log.lux} lux`, inline: true },
                        { name: "Suitable Range", value: "400 - 600 lux", inline: true }
                    ],
                    timestamp: new Date().toISOString()
                }
            }
            payload.embeds.push(brightness_payload);
        }


        await axios.post(webhookUrl, payload);
        res.status(201).json({success: true, data: log});
    } catch (err) {
        const webhookUrl = 'https://discord.com/api/webhooks/1310846461486825542/XIFm1JnAAwwBdIZlpvLttDOEnxtI961jADtggUJNH4D2OptqC3ChXx5UorvaFZSB3Des';
        const payload = {
            content: "<@299769980600320001>",
            embeds: [{
                title: "**Error**",
                description: "An error occurred while sending data to the server.",
                color: 16711680,
                fields: [
                    { name: "Error", value: err.message, inline: true }
                ],
                timestamp: new Date().toISOString()
            }]
        };
        await axios.post(webhookUrl, payload);
        res.status(400).json({success: false, message: err.message});
    }
}