-- Juic'd database version 1.0 by Evan Huang
-- * = not null column variables

-- Schema juicd

DROP DATABASE juicd;
CREATE DATABASE juicd;
USE juicd;


-- ENTITY customers
-- Describes all customer data for juic'd
CREATE TABLE IF NOT EXISTS juicd.customers (
  customerID INT(12) NOT NULL AUTO_INCREMENT,			-- unique PRIMARY KEY (assigned ID)*
  cnameFirst VARCHAR(45) NOT NULL,						-- First Name*
  cnameLast VARCHAR(45) NOT NULL,						-- Last Name*
  email VARCHAR(254) NULL DEFAULT NULL,					-- Email address (If NULL, do not send newsletter)
  points INT(8) NOT NULL,								-- Points*
  PRIMARY KEY (customerID),								
  UNIQUE INDEX customerID_UNIQUE (customerID ASC)
)
ENGINE = InnoDB;



-- ENTITY staff
-- Describes all current employees of Juic'd
CREATE TABLE IF NOT EXISTS juicd.staff (
  staffID INT(12) NOT NULL AUTO_INCREMENT,			    -- unique PRIMARY KEY (assigned ID)*
  snameFirst VARCHAR(45) NOT NULL,						-- First Name*
  snameLast VARCHAR(45) NOT NULL,						-- Last Name*
  staffRank VARCHAR(30) NOT NULL,						-- Rank, i.e. Manager, Trainee etc.*
  reportsTo INT(12) NULL DEFAULT NULL,					-- Name of manager, (if rank is manager then leave as NULL)
  PRIMARY KEY (staffID),
  UNIQUE INDEX staffID_UNIQUE (staffID ASC),
    FOREIGN KEY (reportsTo)								-- derived from relation between managers and employees (reportsTo)
    REFERENCES juicd.staff (staffID)
)
ENGINE = InnoDB;


-- ENTITY outlets
-- Describes all outlets of juic'd
CREATE TABLE IF NOT EXISTS juicd.outlets (
  outletID INT(8) NOT NULL AUTO_INCREMENT,		-- unique PRIMARY KEY (assigned ID) *
  outletName VARCHAR(45) NOT NULL,				-- Name of shopping mall outlet is located within*
  managerID INT(12) NOT NULL,					-- ID of manager currently running outlet*
  PRIMARY KEY (outletID),
  UNIQUE INDEX outletID_UNIQUE (outletID ASC),
    FOREIGN KEY (managerID)						-- derived from relation between managers and outlets (managerOf)
    REFERENCES juicd.staff (staffID)
)
ENGINE = InnoDB;

-- RELATION worksAt
-- N-to-N relationship worksAt
-- i.e. Employee A worksAt Outlet 1
CREATE TABLE IF NOT EXISTS juicd.worksAt (		
  staffID INT(12) NOT NULL,						-- PRIMARY KEY (Staff ID <- staff)*
  outletID INT(8) NOT NULL,						-- PRIMARY KEY (Outlet ID <- outlets)*
  percentage DOUBLE NOT NULL,					-- percentage of time spent at outlet X*
  PRIMARY KEY (staffID, outletID),	 
    FOREIGN KEY (staffID)
    REFERENCES juicd.staff (staffID),
    FOREIGN KEY (outletID)
    REFERENCES juicd.outlets (outletID)
)
ENGINE = InnoDB;

-- ENTITY orders
-- Describes all orders placed to date
CREATE TABLE IF NOT EXISTS juicd.orders (
  orderID INT(12) NOT NULL,						-- unique PRIMARY KEY (assigned ID) *
  customerID INT(12) NOT NULL,					-- SECONDARY KEY (Customer ID <- customers) *
  staffID INT(12) NOT NULL,						-- SECONDARY KEY (Staff ID <- staff, outlets) *
  outletID INT(8) NOT NULL,						-- SECONDARY KEY (Outlet ID <- staff, outlets) *
  timedate DATETIME NOT NULL,					-- Time and Date of transaction *
  day ENUM('Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday') NOT NULL,	-- Day *
  PRIMARY KEY (orderID),
  UNIQUE INDEX salesID_UNIQUE (orderID ASC),			-- derived from Relation between orders and customers (makesOrder)
    FOREIGN KEY (customerID)
    REFERENCES juicd.customers (customerID),			-- derived from Relation between orders and staff/outlet (servedBy)
    FOREIGN KEY (staffID , outletID)
    REFERENCES juicd.worksAt (staffID , outletID)		-- Aggregation (worksAt relationship set)
)
ENGINE = InnoDB;

-- ENTITY items
-- Describes all items sold at juic'd (both juices and non-juices)
CREATE TABLE IF NOT EXISTS juicd.items (
  itemID INT(12) NOT NULL,								-- unique PRIMARY KEY (assigned ID) *
  itemName VARCHAR(45) NOT NULL,						-- item name*
  itemPrice DOUBLE NOT NULL,							-- price in AUD of item (as fixed price sold at all outlets)*
  PRIMARY KEY (itemID),
  UNIQUE INDEX itemID_UNIQUE (itemID ASC)
)
ENGINE = InnoDB;

-- ENTITY ingredientsJuice
-- Describes all possible ingredients for use in juices
CREATE TABLE IF NOT EXISTS juicd.ingredientsJuice (
  ingredientName VARCHAR(45) NOT NULL,				  -- unique PRIMARY KEY (Ingredient Name)*
  unitPrice DOUBLE NOT NULL,						  -- unit price in AUD of ingredient (per ml)*
  PRIMARY KEY (ingredientName),
  UNIQUE INDEX ingredientName_UNIQUE (ingredientName ASC)
)
ENGINE = InnoDB;

-- RELATION has[Items]
-- N-to-N relationship hasItems
-- i.e. Order 1 has 3 quantity of item 123
CREATE TABLE IF NOT EXISTS juicd.hasItems ( 	
  orders_orderID INT(12) NOT NULL,				-- PRIMARY KEY (Order ID <- orders)*
  itemID INT(12) NOT NULL,						-- PRIMARY KEY (Item ID <- items) *
  itemQuantity INT(4) NOT NULL,					-- Quantity of item X ordered*
  PRIMARY KEY (orders_orderID, itemID),
    FOREIGN KEY (orders_orderID)
    REFERENCES juicd.orders (orderID),
    FOREIGN KEY (itemID)
    REFERENCES juicd.items (itemID)
)
ENGINE = InnoDB;


-- RELATION isJuice
-- N-to-N relationship isJuice
-- i.e. Item 13 (juice) has quantity 0.3 of ingredient XYZ
CREATE TABLE IF NOT EXISTS juicd.isJuice (		
  itemID INT(12) NOT NULL,					 	-- PRIMARY KEY (item ID <- items)*
  ingredientName VARCHAR(45) NOT NULL,			-- PRIMARY KEY (Name of ingredient <- ingredientsJuice)*
  ingredientQuantity DOUBLE NOT NULL,			-- Quantity of ingredient X ordered (as a percentage)*
  cupSize INT(4) NULL,							-- Size of Cup (in ml). NULL for items which are non-juice items.
  PRIMARY KEY (itemID, ingredientName),
    FOREIGN KEY (itemID)
    REFERENCES juicd.items (itemID),
    FOREIGN KEY (ingredientName)
    REFERENCES juicd.ingredientsJuice (ingredientName)
)
ENGINE = InnoDB;