const mongoose = require('mongoose');

const LogSchema = new mongoose.Schema({
    timestamp: {
        type: Date,
        default: Date.now
    },
    temperature: {
        type: Number,
        required: [true, 'Please add a temperature']
    },
    humidity: {
        type: Number,
        required: [true, 'Please add a humidity']
    },
    lux: {
        type: Number,
        required: [true, 'Please add a lux value']
    },
    smoke: {
        type: Boolean,
        default: false,
        required: [true, 'Please add a smoke value']
    },
    people: {
        type: Number,
        default: 0,
        required: [true, 'Please add the number of people']
    }
});

module.exports = mongoose.model('Log', LogSchema);