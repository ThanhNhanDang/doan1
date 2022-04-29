const mongoose = require("mongoose");
const Schema = mongoose.Schema;

// Define collection and schema for Business
let Relay2 = new Schema(
  {
    topic: {
      type: String,
    },
    idrl: {
      type: Number,
    },
    idgw: {
      type: Number,
    },

    time: {
      type: String,
    },
    iddv: {
      type: Number,
    },

    state: {
      type: String,
    },
  },
  {
    collection: "relay2",
  }
);

module.exports = mongoose.model("Relay2", Relay2);
