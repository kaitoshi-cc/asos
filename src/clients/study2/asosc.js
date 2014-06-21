function log_clear(){
	document.getElementById("log").innerHTML = "";
}

function log(level, msg){
	now = new Date();
	timestamp =  ("000" + now.getFullYear()).slice(-4)+"/"+("0"+(now.getMonth()+1)).slice(-2)+"/"+("0"+now.getDay()).slice(-2)+" ";
	timestamp += ("0"+now.getHours()).slice(-2)+":"+("0"+now.getMinutes()).slice(-2)+":"+("0"+now.getSeconds()).slice(-2)+".";
	timestamp += ("00"+now.getMilliseconds()).slice(-3);
  
	document.getElementById("log").innerHTML += "<tt>" + timestamp + " [" + level + "] " + msg + "</tt><br>";
}
var short_log = true;

function ASOS_message(){}

ASOS_message.prototype.message_type_string = function(){
  switch(this.message_type){
  case 0x03:  return "browse model command";
  case 0x83:  return "browse model response";

  case 0x04:  return "update model command";
  case 0x84:  return "update model response";

  case 0x05:  return "pop message command";
  case 0x85:  return "pop message response";
  case 0x06:  return "push message command";
  case 0x86:  return "push message response";

  case 0x07:  return "create object command";
  case 0x87:  return "create object response";
  case 0x0a:  return "delete object command";
  case 0x8a:  return "delete object response";

  case 0x01:  return "model publish";
  case 0x09:  return "register model subscription command";
  case 0x89:  return "register model subscription response";
  case 0x0c:  return "cancel model subscription command";
  case 0x8c:  return "cancel model subscription response";
  
  case 0x0d:  return "captured message";
  case 0x0e:  return "register message capture command";
  case 0x8e:  return "register message capture response";
  case 0x0f:  return "cancel message capture command";
  case 0x8f:  return "cancel message capture response";
  default:    return "(unknown)";
  }
}

ASOS_message.prototype.response_state_string = function(){
  switch(this.response_state){
  case 0x00:  return "success";
  case 0x01:  return "field or object not found";
  case 0x02:  return "timeout";
  case 0x03:  return "access denied";
  case 0x04:  return "access limitation over";
  case 0x05:  return "same command already done";
  case 0x80:  return "previous model revision is too old";
  case 0x81:  return "previous model revision is invalid (future revision)";
  case 0x82:  return "model size is too big";
  case 0x83:  return "target model revision is too old";
  case 0x84:  return "target model revision is invalid (future revision)";
  case 0x85:  return "message size is too big";
  case 0x86:  return "queue is full";
  case 0x87:  return "queue is empty";
  case 0x88:  return "object already exist";
  case 0x89:  return "server resource error";
  default:    return "(unknown)";
  }
}

ASOS_message.prototype.object_state_string = function(){
  switch(this.object_state){
  case 0x00:  return "object not acvite";
  case 0x01:  return "object active";
  case 0x02:  return "object busy";
  default:    return "(unknown)";
  }
}

ASOS_message.prototype.Print = function(){
	log("info", "[ASOS] Message TYPE.. : " + this.message_type_string());
	
	// ---------------------------
	// *** for shorten log. ****
	// ---------------------------
	if(short_log) return;
	
	log("info", "[ASOS] Message ID.... : " 
		+ ("0"+this.message_identification0.toString(16)).slice(-2)
		+ ("0"+this.message_identification1.toString(16)).slice(-2));
	log("info", "[ASOS] ObjectField ID : " + this.object_field_identification);
	log("info", "[ASOS] ObjectField Ext: " + this.object_field_extension);
	log("info", "[ASOS] Object ID..... : " + this.object_identification);

	switch(this.message_type){
  	case 0x01: // "model publish";
		log("info", "[ASOS] Object State.. : " + this.object_state_string());
		log("info", "[ASOS] Key count      : " + this.key_count);
		log("info", "[ASOS] Node-ID count  : " + this.node_id_count);
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		log("info", "[ASOS] Model Data.... : " + this.model_data );
		break;
    case 0x83: // "browse model response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		log("info", "[ASOS] Object State.. : " + this.object_state_string());
		log("info", "[ASOS] Key count      : " + this.key_count);
		log("info", "[ASOS] Node-ID count  : " + this.node_id_count);
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		log("info", "[ASOS] Model Data.... : " + this.model_data );
		break;
    case 0x84: // "update model response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		break;
    case 0x85: // "pop message response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		log("info", "[ASOS] Message....... : " + this.message );
		break;
    case 0x86: // "push message response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;
    case 0x87: // "create object response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		break;
    case 0x89: // "register model subscription response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;
    case 0x8a: // "delete object response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;
    case 0x8c: // "cancel model subscription response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;

    case 0x0d: // "captured message";
		log("info", "[ASOS] Model Revision : " + this.model_revision);
		log("info", "[ASOS] Message....... : " + this.message );
		break;

    case 0x8e: // "register message capture response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;
    case 0x8f: // "cancel message capture response";
		log("info", "[ASOS] Response State : " + this.response_state_string());
		break;

    default:   // "(unknown)";
	}

}

ASOS_message.prototype.get_model_revision = function(c0,c1,c2,c3,c4,c5,c6,c7){
	return (c0 * 0x0100000000000000) + (c1 * 0x0001000000000000)+ (c2 * 0x0000010000000000)+ (c3 * 0x0000000100000000)
		 + (c4 * 0x0000000001000000) + (c5 * 0x0000000000010000)+ (c6 * 0x0000000000000100)+ (c7 * 0x0000000000000001);
}

ASOS_message.prototype.get_message_category = function(){
	switch(this.message_type){
		           case 0x01:            case 0x83: case 0x86: 
		           case 0x89:                       case 0x8c: case 0x0d: case 0x8e: case 0x8f: 
		return "responce_consumer";
		break;
		
		case 0x84: case 0x85: case 0x87: case 0x8a:
		return "responce_producer";
		break;
		                                 case 0x03: case 0x04: case 0x05: case 0x06: case 0x07: 
		           case 0x09: case 0x0a:            case 0x0c:            case 0x0e: case 0x0f: 
		return "command";
		break;
		default:
		return "unknown";
		break;
	}
}

ASOS_message.prototype.Parse = function(message){

	this.protocol_type                      = message[0];
	
	this.message_type                       = message[2];
	this.wait_time_for_response             = message[3];
	this.registration_lifetime              = message[4];
	this.lifetime_overwrite_flag            = (message[5] & 0x80 == 0x80)?1:0;
	this.private_flag                       = (message[5] & 0x40 == 0x40)?1:0;
  	
	this.message_identification0            = message[6];
	this.message_identification1            = message[7];
	
	this.object_field_identification_length = message[8];
	this.object_identification_length       = message[9];
	
	this.object_field_identification = "";
	for(i=0;i<this.object_field_identification_length;i++){
		this.object_field_identification += ascii(message[10+i]);
	}

	this.object_field_extension = "";
	for(i=0;i<16;i++){
		this.object_field_extension += ascii(message[10+this.object_field_identification_length+i]);
	}

	this.object_identification = "";
	for(i=0;i<this.object_identification_length;i++){
		this.object_identification += ascii(message[26+this.object_field_identification_length+i]);
	}
	
	_index = 26 + this.object_field_identification_length + this.object_identification_length;
	
	switch(this.message_type){
  	case 0x01: // "model publish";
		this.object_state = message[_index];  _index++;
		
		this.key_count     = message[_index];  _index++;
		this.node_id_count = message[_index];  _index++;
		for(i=0; i<this.key_count; i++){
			_index+=16;
		}
		for(i=0; i<this.node_id_count; i++){
			_index+=16;
		}
		
		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		_index+=8;
		this.model_data = "";
		while(_index < message.length){
			this.model_data += ascii(message[_index]); _index++;
		}
		break;
    case 0x83: // "browse model response";
		this.response_state = message[_index];  _index++;
		this.object_state = message[_index];  _index++;

		this.key_count     = message[_index];  _index++;
		this.node_id_count = message[_index];  _index++;
		for(i=0; i<this.key_count; i++){
			_index+=16;
		}
		for(i=0; i<this.node_id_count; i++){
			_index+=16;
		}

		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		_index+=8;
		this.model_data = "";
		while(_index < message.length){
			this.model_data += ascii(message[_index]); _index++;
		}
		break;
    case 0x84: // "update model response";
		this.response_state = message[_index];  _index++;
		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		break;
    case 0x85: // "pop message response";
		this.response_state = message[_index];  _index++;
		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		_index+=8;
		this.message = "";
		while(_index < message.length){
			this.message += ascii(message[_index]); _index++;
		}
		break;
    case 0x86: // "push message response";
		this.response_state = message[_index];  _index++;
		break;
    case 0x87: // "create object response";
		this.response_state = message[_index];  _index++;
		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		_index+=8;
		break;
    case 0x89: // "register model subscription response";
		this.response_state = message[_index];  _index++;
		break;
    case 0x8a: // "delete object response";
		this.response_state = message[_index];  _index++;
		break;
    case 0x8c: // "cancel model subscription response";
		this.response_state = message[_index];  _index++;
		break;

    case 0x0d: // "captured message";
		this.model_revision = this.get_model_revision(message[_index+0], message[_index+1], message[_index+2], message[_index+3], 
								   	 				  message[_index+4], message[_index+5], message[_index+6], message[_index+7] );
		_index+=8;
		this.message = "";
		while(_index < message.length){
			this.message += ascii(message[_index]); _index++;
		}
		break;

    case 0x8e: // "register message capture response";
		this.response_state = message[_index];  _index++;
		break;
    case 0x8f: // "cancel message capture response";
		this.response_state = message[_index];  _index++;
		break;
    default:   // "(unknown)";
	}
	
}

function ascii(c){
	switch(c){
	case 0x20: return " ";case 0x21: return "!";case 0x22: return "\"";case 0x23: return "#";
    case 0x24: return "$";case 0x25: return "%";case 0x26: return "&";case 0x27: return "'";
    case 0x28: return "(";case 0x29: return ")";case 0x2A: return "*";case 0x2B: return "+";
    case 0x2C: return ",";case 0x2D: return "-";case 0x2E: return ".";case 0x2F: return "/";
    case 0x30: return "0";case 0x31: return "1";case 0x32: return "2";case 0x33: return "3";
    case 0x34: return "4";case 0x35: return "5";case 0x36: return "6";case 0x37: return "7";
    case 0x38: return "8";case 0x39: return "9";case 0x3A: return ":";case 0x3B: return ";";
    case 0x3C: return "<";case 0x3D: return "=";case 0x3E: return ">";case 0x3F: return "?";
    case 0x40: return "@";case 0x41: return "A";case 0x42: return "B";case 0x43: return "C";
    case 0x44: return "D";case 0x45: return "E";case 0x46: return "F";case 0x47: return "G";
    case 0x48: return "H";case 0x49: return "I";case 0x4A: return "J";case 0x4B: return "K";
    case 0x4C: return "L";case 0x4D: return "M";case 0x4E: return "N";case 0x4F: return "O";
    case 0x50: return "P";case 0x51: return "Q";case 0x52: return "R";case 0x53: return "S";
    case 0x54: return "T";case 0x55: return "U";case 0x56: return "V";case 0x57: return "W";
    case 0x58: return "X";case 0x59: return "Y";case 0x5A: return "Z";case 0x5B: return "[";
    case 0x5C: return "\\";case 0x5D: return "]";case 0x5E: return "^";case 0x5F: return "_";
    case 0x60: return "`";case 0x61: return "a";case 0x62: return "b";case 0x63: return "c";
    case 0x64: return "d";case 0x65: return "e";case 0x66: return "f";case 0x67: return "g";
    case 0x68: return "h";case 0x69: return "i";case 0x6A: return "j";case 0x6B: return "k";
    case 0x6C: return "l";case 0x6D: return "m";case 0x6E: return "n";case 0x6F: return "o";
    case 0x70: return "p";case 0x71: return "q";case 0x72: return "r";case 0x73: return "s";
    case 0x74: return "t";case 0x75: return "u";case 0x76: return "v";case 0x77: return "w";
    case 0x78: return "x";case 0x79: return "y";case 0x7A: return "z";case 0x7B: return "{";
    case 0x7C: return "|";case 0x7D: return "}";case 0x7E: return "~";
	}
}
function atoc(a){
	switch(a){
	case " ": return 0x20; case "!": return 0x21; case "\"": return 0x22; case "#": return 0x23; 
	case "$": return 0x24; case "%": return 0x25; case "&": return 0x26; case "'": return 0x27; 
	case "(": return 0x28; case ")": return 0x29; case "*": return 0x2A; case "+": return 0x2B; 
	case ",": return 0x2C; case "-": return 0x2D; case ".": return 0x2E; case "/": return 0x2F; 
	case "0": return 0x30; case "1": return 0x31; case "2": return 0x32; case "3": return 0x33; 
	case "4": return 0x34; case "5": return 0x35; case "6": return 0x36; case "7": return 0x37; 
	case "8": return 0x38; case "9": return 0x39; case ":": return 0x3A; case ";": return 0x3B; 
	case "<": return 0x3C; case "=": return 0x3D; case ">": return 0x3E; case "?": return 0x3F; 
	case "@": return 0x40; case "A": return 0x41; case "B": return 0x42; case "C": return 0x43; 
	case "D": return 0x44; case "E": return 0x45; case "F": return 0x46; case "G": return 0x47; 
	case "H": return 0x48; case "I": return 0x49; case "J": return 0x4A; case "K": return 0x4B; 
	case "L": return 0x4C; case "M": return 0x4D; case "N": return 0x4E; case "O": return 0x4F; 
	case "P": return 0x50; case "Q": return 0x51; case "R": return 0x52; case "S": return 0x53; 
	case "T": return 0x54; case "U": return 0x55; case "V": return 0x56; case "W": return 0x57; 
	case "X": return 0x58; case "Y": return 0x59; case "Z": return 0x5A; case "[": return 0x5B; 
	case "\\": return 0x5C; case "]": return 0x5D; case "^": return 0x5E; case "_": return 0x5F; 
	case "`": return 0x60; case "a": return 0x61; case "b": return 0x62; case "c": return 0x63; 
	case "d": return 0x64; case "e": return 0x65; case "f": return 0x66; case "g": return 0x67; 
	case "h": return 0x68; case "i": return 0x69; case "j": return 0x6A; case "k": return 0x6B; 
	case "l": return 0x6C; case "m": return 0x6D; case "n": return 0x6E; case "o": return 0x6F; 
	case "p": return 0x70; case "q": return 0x71; case "r": return 0x72; case "s": return 0x73; 
	case "t": return 0x74; case "u": return 0x75; case "v": return 0x76; case "w": return 0x77; 
	case "x": return 0x78; case "y": return 0x79; case "z": return 0x7A; case "{": return 0x7B; 
	case "|": return 0x7C; case "}": return 0x7D; case "~": return 0x7E;
	}
}


function event_string(event){
	return event.type + "," + event.target + "," + event.currentTarget + "," + event.eventPhase  + "," + event.bubbles  + "," +  event.cancelable + "," + event.timeStamp;
}


// --------------------------------------------------------------------------
// Session Starter
// --------------------------------------------------------------------------
function StartSession(asos){
	log("info", "start ASOS Session");
	
	for( var fid in asos.object_fields ){
		log("info", "Object Field ID = " + fid);
		for( var poid in asos.object_fields[fid].producer_objects){
			log("info", "Produced Object ID = " + poid);
			asos.protocol.CreateObject(fid, asos.object_fields[fid].producer_objects[poid].field_extension, asos.object_fields[fid].producer_objects[poid].id);
		}
		for( var coid in asos.object_fields[fid].consumer_objects){
			log("info", "Consumed Object ID = " + coid);
			asos.object_fields[fid].consumer_objects[coid].subscribe();
		}
	}

	asos.startPong();

}

// --------------------------------------------------------------------------
function ASOS_Object(){
	this.pop_message_id = 0;
}
// --------------------------------------------------------------------------
ASOS_Object.prototype.set_field_extension = function(field_extension) { this.field_extension = field_extension; }
ASOS_Object.prototype.set_id              = function(id)              { this.id = id; }
ASOS_Object.prototype.set_model           = function(model)           { this.model = model; }
ASOS_Object.prototype.set_field           = function(field)           { this.field = field; }

ASOS_Object.prototype.push = function(message){
	var json_message = JSON.stringify(message);
	if(!short_log) log("info", "MESSAGE(JSON): " + json_message);
	this.field.asos.protocol.PushMessage(this.field.id, this.field_extension, this.id, this.model_revision, json_message);
}

ASOS_Object.prototype.update = function(){
	var json_object = JSON.stringify(this.model);
	if(!short_log) log("info", "MODEL(JSON): " + json_object);
	this.field.asos.protocol.UpdateModel(this.field.id, this.field_extension, this.id, this.model_revision,json_object);
}

ASOS_Object.prototype.pop = function(continue_flag){
	timeout = 5;

	if(this.pop_message_id == 0){
		this.pop_message_id = this.field.asos.protocol.get_next_message_id();
		// TODO : Reserve pop_message_id in this.field.asos.protocol in order to prevent re-use this message id for the other purpose.
	}
	
	this.field.asos.protocol.PopMessage(this.field.id, this.field_extension, this.id, timeout, this.pop_message_id);
	var object = this;
	if(continue_flag == true) setTimeout( function(){ object.pop(true); } , timeout*1000);
}

ASOS_Object.prototype.subscribe = function(){
	this.field.asos.protocol.RegisterModelSubscription(this.field.id, this.field_extension, this.id);
}

ASOS_Object.prototype.ProducerProcess = function(asos_msg){
	switch(asos_msg.message_type){

		case 0x01: //"model publish"
			this.model_revision = asos_msg.model_revision;
			this.model = JSON.parse(asos_msg.model_data);
			this.onModelUpdated();
		break;

		case 0x84: //"update model response"
			if( asos_msg.response_state == 0x00 || asos_msg.response_state == 0x80 || asos_msg.response_state == 0x81){
				this.model_revision = asos_msg.model_revision;
			}
		break;

		case 0x85: //"pop message response"
			if( asos_msg.response_state == 0x00){
				message = JSON.parse(asos_msg.message);
				this.onMessagePoped(asos_msg.model_revision, message);
				this.pop(false);
			}
		break;

		case 0x87: //"create object response"
			if( asos_msg.response_state == 0x00 || asos_msg.response_state == 0x88){
				this.model_revision = asos_msg.model_revision;
				if(asos_msg.response_state == 0x00) this.update();
				this.subscribe();
				this.pop(true);
			}
		break;

		case 0x8a: //"delete object response"
		break;
	}
}

ASOS_Object.prototype.onModelUpdated = function(){
	var str = "";
	for(var p in this.model){
		str += p + " = " + this.model[p] + " / ";
	}

	if(!short_log) log("info", "onModelUpdated (rev:" + this.model_revision + ") : " + str);
}

ASOS_Object.prototype.onMessagePoped = function(target_revision, message){
	var str = "";
	for(var p in message){
		str += p + " = " + message[p] + " / ";
	}

	if(!short_log) log("info", "onMessagePoped (rev:" + target_revision + ") : " + str);
}

ASOS_Object.prototype.ConsumerProcess = function(asos_msg){
	switch(asos_msg.message_type){
		case 0x01: //"model publish"
			this.object_state = asos_msg.object_state;
			this.model_revision = asos_msg.model_revision;
			this.model = JSON.parse(asos_msg.model_data);
			this.onModelUpdated();
		break;
		
		case 0x83: //"browse model response"
			if( asos_msg.response_state == 0x00 ){
	   			this.object_state = asos_msg.object_state;
				this.model_revision = asos_msg.model_revision;
				this.model = JSON.parse(asos_msg.model_data);
				this.onModelUpdated();
			}
		break;
		
		case 0x86: //"push message response"
		break;
		
		case 0x89: //"register model subscription response"
		break;
		
		case 0x8c: //"cancel model subscription response"
		break;
		
		case 0x0d: //"captured message"
		break;
		
		case 0x8e: //"register message capture response"
		break;
		
		case 0x8f: //"cancel message capture response"
		break;
	}
}

// --------------------------------------------------------------------------
function ASOS_ObjectField(){
	this.producer_objects = {};
	this.consumer_objects = {};
}
// --------------------------------------------------------------------------
ASOS_ObjectField.prototype.set_id   = function(id)   { this.id = id; }
ASOS_ObjectField.prototype.set_asos = function(asos) { this.asos = asos; }

ASOS_ObjectField.prototype.add_producer_object = function(obj) { 
	this.producer_objects[obj.id] = obj;
	obj.set_field(this);
}
ASOS_ObjectField.prototype.add_consumer_object = function(obj) { 
	this.consumer_objects[obj.id] = obj;
	obj.set_field(this);
}

// --------------------------------------------------------------------------
function ASOS_Protocol(){
	this.message_id = Math.floor( Math.random() * 0xffff );
}
// --------------------------------------------------------------------------

ASOS_Protocol.prototype.get_next_message_id = function(){
	this.message_id = (this.message_id+1) % 0x10000;
	return this.message_id;
}

ASOS_Protocol.prototype.RegisterModelSubscription = function(field_id, field_ext, object_id){
	var original_array = [0x01,0x00, 0x09,0x00,0x3c,0x00];
	var mgsid = this.get_next_message_id();
	original_array.push(Math.floor(mgsid/0x0100));
	original_array.push(Math.floor(mgsid%0x0100));
	original_array.push(field_id.length);
	original_array.push(object_id.length);
	for(i=0; i<field_id.length; i++)	original_array.push(atoc(field_id[i]));
	for(i=0; i<16; i++)	                original_array.push(atoc(field_ext[i]));
	for(i=0; i<object_id.length; i++)	original_array.push(atoc(object_id[i]));
	
	original_array.push(0x00);  // Key flag
	
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);
}

ASOS_Protocol.prototype.CreateObject = function(field_id, field_ext, object_id){
	var original_array = [0x01,0x00, 0x07,0x05,0x00,0x00];
	var mgsid = this.get_next_message_id();
	original_array.push(Math.floor(mgsid/0x0100));
	original_array.push(Math.floor(mgsid%0x0100));
	original_array.push(field_id.length);
	original_array.push(object_id.length);
	for(i=0; i<field_id.length; i++)	original_array.push(atoc(field_id[i]));
	for(i=0; i<16; i++)	                original_array.push(atoc(field_ext[i]));
	for(i=0; i<object_id.length; i++)	original_array.push(atoc(object_id[i]));
	
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);
}

ASOS_Protocol.prototype.PopMessage = function(field_id, field_ext, object_id, timeout, message_id){
	var original_array = [0x01,0x00, 0x05];
	if(timeout > 0xFF) timeout = 0xFF;
	original_array.push(timeout);
	original_array.push(0x00);
	original_array.push(0x00);
	var mgsid = message_id;
	original_array.push(Math.floor(mgsid/0x0100));
	original_array.push(Math.floor(mgsid%0x0100));
	original_array.push(field_id.length);
	original_array.push(object_id.length);
	for(i=0; i<field_id.length; i++)	original_array.push(atoc(field_id[i]));
	for(i=0; i<16; i++)	                original_array.push(atoc(field_ext[i]));
	for(i=0; i<object_id.length; i++)	original_array.push(atoc(object_id[i]));
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);
}

ASOS_Protocol.prototype.PushMessage = function(field_id, field_ext, object_id, revision, message){
	var original_array = [0x01,0x00, 0x06,0x05,0x00,0x00];
	var mgsid = this.get_next_message_id();
	original_array.push(Math.floor(mgsid/0x0100));
	original_array.push(Math.floor(mgsid%0x0100));
	original_array.push(field_id.length);
	original_array.push(object_id.length);
	for(i=0; i<field_id.length; i++)	original_array.push(atoc(field_id[i]));
	for(i=0; i<16; i++)	                original_array.push(atoc(field_ext[i]));
	for(i=0; i<object_id.length; i++)	original_array.push(atoc(object_id[i]));
	
	original_array.push(0x00);  // Key flag
	
	original_array.push( ( revision & 0xFF00000000000000) / 0x0100000000000000 );
	original_array.push( ( revision & 0x00FF000000000000) / 0x0001000000000000 );
	original_array.push( ( revision & 0x0000FF0000000000) / 0x0000010000000000 );
	original_array.push( ( revision & 0x000000FF00000000) / 0x0000000100000000 );
	original_array.push( ( revision & 0x00000000FF000000) / 0x0000000001000000 );
	original_array.push( ( revision & 0x0000000000FF0000) / 0x0000000000010000 );
	original_array.push( ( revision & 0x000000000000FF00) / 0x0000000000000100 );
	original_array.push( ( revision & 0x00000000000000FF) / 0x0000000000000001 );
	
	for(i=0; i<message.length; i++)	original_array.push(atoc(message[i]));
	
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);

}

ASOS_Protocol.prototype.UpdateModel = function(field_id, field_ext, object_id, revision, model_data){
	var original_array = [0x01,0x00, 0x04,0x05,0x00,0x00];
	var mgsid = this.get_next_message_id();
	original_array.push(Math.floor(mgsid/0x0100));
	original_array.push(Math.floor(mgsid%0x0100));
	original_array.push(field_id.length);
	original_array.push(object_id.length);
	for(i=0; i<field_id.length; i++)	original_array.push(atoc(field_id[i]));
	for(i=0; i<16; i++)	                original_array.push(atoc(field_ext[i]));
	for(i=0; i<object_id.length; i++)	original_array.push(atoc(object_id[i]));
	
	original_array.push(0x00);  // Keys
	original_array.push(0x00);  // Node-IDs
	
	
	original_array.push( ( revision & 0xFF00000000000000) / 0x0100000000000000 );
	original_array.push( ( revision & 0x00FF000000000000) / 0x0001000000000000 );
	original_array.push( ( revision & 0x0000FF0000000000) / 0x0000010000000000 );
	original_array.push( ( revision & 0x000000FF00000000) / 0x0000000100000000 );
	original_array.push( ( revision & 0x00000000FF000000) / 0x0000000001000000 );
	original_array.push( ( revision & 0x0000000000FF0000) / 0x0000000000010000 );
	original_array.push( ( revision & 0x000000000000FF00) / 0x0000000000000100 );
	original_array.push( ( revision & 0x00000000000000FF) / 0x0000000000000001 );
	
	for(i=0; i<model_data.length; i++)	original_array.push(atoc(model_data[i]));
	
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);
}


ASOS_Protocol.prototype.Process = function(message){
	if(message.length <= 8) return;
	asos_msg = new ASOS_message();
	asos_msg.Parse(message);
	asos_msg.Print();
	var object;
	
	if(asos_msg.get_message_category() == "responce_consumer" || asos_msg.get_message_category() == "responce_producer" ){
		if(asos_msg.object_field_identification in this.sock.asos.object_fields ){
			if(asos_msg.get_message_category() == "responce_producer" || asos_msg.message_type == 0x01 ){
				if(asos_msg.object_identification in this.sock.asos.object_fields[asos_msg.object_field_identification].producer_objects ){
					object = this.sock.asos.object_fields[asos_msg.object_field_identification].producer_objects[asos_msg.object_identification];
					object.ProducerProcess(asos_msg);
				}else{
					log("error", "ASOS_Protocol.prototype.Process: There are no produced object (ID: " + asos_msg.object_identification + " in the Client.");
				}
			}
			if(asos_msg.get_message_category() == "responce_consumer"){
				if(asos_msg.object_identification in this.sock.asos.object_fields[asos_msg.object_field_identification].consumer_objects ){
					object = this.sock.asos.object_fields[asos_msg.object_field_identification].consumer_objects[asos_msg.object_identification];
					object.ConsumerProcess(asos_msg);
				}else{
					log("error", "ASOS_Protocol.prototype.Process: There are no consumed object (ID: " + asos_msg.object_identification + " in the Client.");
				}
			}
		
		}else{
			log("error", "ASOS_Protocol.prototype.Process: There are no fieid (ID: " + asos_msg.object_field_identification + " in the Client.");
		}
	}else{
		log("error", "ASOS_Protocol.prototype.Process: Wrong message type (" + asos_msg.get_message_category() + ")");
	}
}

ASOS_Protocol.prototype.sock_onmessage_as_array_buffer = function(event){
	array = new Uint8Array(this.result);
	str = "";
	for(i = 0 ; i<array.length; i++  ){
		str += ("0"+array[i].toString(16)).slice(-2);
		if(i > 32){
			str += "... ";
			break;
		}
	}
	if(!short_log ) log("info", "MESSAGE_AS_ArrayBuffer: (" + array.length + ")" + str);

	this.protocol.Process(array);
}

// --------------------------------------------------------------------------
function ASOS_Clinet(){
	this.object_fields = {};
}
// --------------------------------------------------------------------------

ASOS_Clinet.prototype.sock_onopen = function(event){
	log("info", "socket OPEN with " + event_string(event));
	log("info", "socket param: " + this.bufferedAmount);
	StartSession(this.asos);
//	setTimeout(test(this),5000);
}
ASOS_Clinet.prototype.sock_onerror = function(event){
	log("info", "socket ERROR with " + event_string(event));
	log("info", "socket param: " + this.bufferedAmount);
}
ASOS_Clinet.prototype.sock_onclose = function(event){
	log("info", "socket CLOSE with " + event_string(event));
	log("info", "socket param: " + this.bufferedAmount);
}
ASOS_Clinet.prototype.sock_onmessage = function(event){
	if(!short_log) log("info", "socket receive with " + event_string(event));
	if(!short_log) log("info", "MESSAGE: " + event.data);

	freader	= new FileReader();
	freader.protocol = this.asos.protocol;
	freader.onloadend = this.asos.protocol.sock_onmessage_as_array_buffer;
	freader.readAsArrayBuffer(event.data);
}


ASOS_Clinet.prototype.sendPong = function(){
	var original_array = [0x02, 0x00, 0x01, 0x00, 0x00, 0x00];
	ary_u8 = new Uint8Array(original_array);
	blob   = new Blob([ary_u8] , {type:"application/octet-stream"}); 
	this.sock.send(blob);
}

ASOS_Clinet.prototype.startPong = function(){
	this.sendPong();
	var asos = this;
	setTimeout( function(){ asos.startPong(); } , 50*1000);
}

ASOS_Clinet.prototype.add_object_field = function(object_field){
	this.object_fields[object_field.id] = object_field;
	object_field.set_asos(this);
}

ASOS_Clinet.prototype.start = function(ws_url){
	
	this.sock = new WebSocket(ws_url, ["asos"]);
	this.sock.asos = this;
	this.sock.onopen    = this.sock_onopen;
	this.sock.onerror   = this.sock_onerror;
	this.sock.onclose   = this.sock_onclose;
	this.sock.onmessage = this.sock_onmessage;

	this.protocol = new ASOS_Protocol();
	this.protocol.sock = this.sock;
}

// --------------------------------------------------------------------------
