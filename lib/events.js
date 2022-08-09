export default class EventEmitter {
	constructor() {
		this.__listeners={};
	}

	on=(event, fn)=>{
		if (!this.__listeners[event])
			this.__listeners[event]=[];

		this.__listeners[event].push(fn);
	}

	off=(event, fn)=>{
		if (!this.__listeners[event])
			return;

		let index=this.__listeners[event].indexOf(fn);
		for (; index>=0; index=this.__listeners[event].indexOf(fn))
			this.__listeners[event].splice(index,1);

		if (!this.__listeners[event].length)
			delete this.__listeners[event];
	}

	emit=(event, ...args)=>{
		if (!this.__listeners[event])
			return;

		for (let fn of this.__listeners[event])
			fn(...args);
	}
}