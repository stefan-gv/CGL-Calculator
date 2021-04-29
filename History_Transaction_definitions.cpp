#ifndef _HISTORY_TRANSACTION_CPP_
#define _HISTORY_TRANSACTION_CPP_

#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

// Constructor
Transaction::Transaction(std::string ticker_symbol, unsigned int day_date, unsigned int month_date, unsigned int year_date, bool buy_sell_trans, unsigned int number_shares, double trans_amount){
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;
  if (buy_sell_trans){trans_type = "Buy";} else{trans_type = "Sell";}
  shares = number_shares;
  amount = trans_amount;
  
  trans_id = assigned_trans_id;
  assigned_trans_id++;

  acb = 0;
  acb_per_share = 0;
  share_balance = 0;
  cgl = 0;

  p_next = nullptr;
}


// Destructor

Transaction::~Transaction(){

}




bool Transaction::operator<(Transaction const &other){
  if (this->year < other.get_year()){return true;}
  else if (this->year == other.get_year() && this->month < other.get_month()){return true;}
  else if (this->year == other.get_year() && this->month == other.get_month() && this->day < other.get_day()){return true;}
  else {{return false;}}
}

std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}




// Constructor
History::History(){
  p_head = nullptr;
}

// Destructor
History::~History(){
  
  Transaction *p_temp = p_head;

  if(p_temp!=nullptr){
    while (p_temp->get_next() != nullptr){
      p_head = p_temp->get_next();
      delete p_temp;
      p_temp = nullptr;
      p_temp = p_head;
    }
    delete p_temp;
    p_temp = nullptr;
    p_head = nullptr;
  }
  
}

void History::read_history(){
  ece150::open_file();
  while(ece150::next_trans_entry()){
    Transaction *new_trans = new Transaction(ece150::get_trans_symbol(), ece150::get_trans_day(), ece150::get_trans_month(), ece150::get_trans_year(), ece150::get_trans_type(), ece150::get_trans_shares(), ece150::get_trans_amount());
    insert(new_trans);
  }
  ece150::close_file();
}

void History::insert(Transaction *p_new_trans){
  if (p_head == nullptr){
    p_head = p_new_trans;
  }else{
    Transaction *p_temp = p_head;
    while (p_temp->get_next()!=nullptr){
      p_temp=p_temp->get_next();
    }
    p_temp->set_next(p_new_trans);
  } 
}

void History::sort_by_date(){
  
  if (p_head == nullptr || p_head->get_next() == nullptr){}
  else{
    Transaction *p_tempOG = p_head;
    
    Transaction *p_sorted = nullptr;

    
    p_head = p_head->get_next();
    p_tempOG->set_next(nullptr);
    p_sorted = p_tempOG;

    while (p_head!=nullptr){
      p_tempOG = p_head;
      p_head = p_head->get_next();
      p_tempOG->set_next(nullptr);

      if (*p_tempOG<*p_sorted){
        p_tempOG->set_next(p_sorted);
        p_sorted = p_tempOG;
      }else{
        Transaction *p_tempSort= p_sorted;
        while((p_tempSort->get_next()!=nullptr) && !(*p_tempOG < *(p_tempSort->get_next()))){
          p_tempSort = p_tempSort->get_next();
        }
        p_tempOG->set_next(p_tempSort->get_next());
        p_tempSort->set_next(p_tempOG);

      }
    }
    p_head = p_sorted;
  }
  
}


void History::update_acb_cgl(){
  Transaction *p_before = p_head;
  Transaction *p_after = p_before->get_next();

  //Setting first transaction assuming "buy"
  p_before->set_share_balance(p_before->get_shares());
  p_before->set_acb(p_before->get_amount());
  p_before->set_acb_per_share(p_before->get_acb()/p_before->get_share_balance());

  while (p_after!=nullptr){
    if (p_after->get_trans_type()){
      p_after->set_share_balance(p_before->get_share_balance()+p_after->get_shares());
      p_after->set_acb(p_before->get_acb()+p_after->get_amount());
      p_after->set_acb_per_share(p_after->get_acb()/p_after->get_share_balance());
    }else{
      p_after->set_share_balance(p_before->get_share_balance()-p_after->get_shares());
      p_after->set_acb(p_before->get_acb()-(p_before->get_acb_per_share()*p_after->get_shares()));
      p_after->set_acb_per_share(p_after->get_acb()/p_after->get_share_balance());

      p_after->set_cgl(p_after->get_amount()-(p_after->get_shares()*p_before->get_acb_per_share()));
    }
    p_before = p_after;
    p_after = p_before->get_next();
  }

}

double History::compute_cgl(unsigned int year){
  Transaction *p_traverse = p_head;
  double amount = 0;
  while (p_traverse != nullptr){
    if (p_traverse->get_year() == year){
      amount += p_traverse->get_cgl();
    }
    p_traverse = p_traverse->get_next();
  }

  return amount;
}

void History::print(){
  Transaction *p_traverse = p_head;
  std::cout<<"========== BEGIN TRANSACTION HISTORY =========="<<std::endl;
  while (p_traverse != nullptr){
    std::string type;
    if (p_traverse->get_trans_type()){type = "Buy";}else{ type = "Sell";}
    std::cout<<p_traverse->get_trans_id()<<"  "<<p_traverse->get_symbol()<<"  "<<p_traverse->get_day()<<"  "<<p_traverse->get_month()<<"  "<<p_traverse->get_year()<<"  "<<type<<"  "<<p_traverse->get_shares()<<"  "<<p_traverse->get_amount()<<"  "<<p_traverse->get_acb()<<"  "<<p_traverse->get_share_balance()<<"  "<<p_traverse->get_acb_per_share()<<"  "<<p_traverse->get_cgl()<<std::endl;
    p_traverse = p_traverse->get_next();
  }
  std::cout<<"========== END TRANSACTION HISTORY =========="<<std::endl;
}
Transaction *History::get_p_head() { return p_head; }


#endif
