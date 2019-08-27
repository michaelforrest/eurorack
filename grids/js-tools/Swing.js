import Easing from 'bezier-easing';
import _ from "lodash";

function swing(amount){
    return amount < 0.5 ? Easing(0,(0.5 - amount),1-(0.5 - amount),1) :  Easing(amount-0.5,0,1,1-(amount-0.5)) 
} 
const count = 16;
const min = 1/36;
const max = 22/24;
const lines = _.range(0,count).map(index => {
    const swingAmount = ((index / count) * (max - min)) + min;
    const swingFunc = swing(swingAmount);
    const line = _.range(0,24,1).map( value => 
        Math.floor(swingFunc(value / 24) * 24)
    )
    .reduce((memo, value)=> [].concat(memo, _.includes(memo, value) ? 99 : value), [])
    .map(value => `${value}`.padStart(2, " "))
    .join(", ")
    return `{ ${line} }, // ${index} swing: ${swingAmount.toFixed(2)}`
 }).join(`\n`)
 console.log(lines);
 