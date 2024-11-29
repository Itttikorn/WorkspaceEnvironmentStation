const express = require('express');
const router = express.Router();
const {getLogs, postLog} = require('../controllers/logs');

router.route('/').get(getLogs).post(postLog);

module.exports = router;