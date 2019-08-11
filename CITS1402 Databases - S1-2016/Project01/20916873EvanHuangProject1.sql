-- Juic'd database version 3.0 by Evan Huang
-- mysql> SOURCE <file location> /20916873EvanHuangProject1.sql

-- Init.

DROP DATABASE juicd;									-- When script is loaded, clear any previous database with the name juicd
CREATE DATABASE juicd;									-- Create new database called juicd
USE juicd;											 	-- Set juicd as the active database

-- ENTITY customers
-- Describes all customer data for juic'd
CREATE TABLE customers (
  customerID INT(12) NOT NULL AUTO_INCREMENT,			-- unique PRIMARY KEY (assigned ID)
  cnameFirst VARCHAR(45) NOT NULL,						-- First Name
  cnameLast VARCHAR(45) NOT NULL,						-- Last Name
  email VARCHAR(254) NULL DEFAULT NULL,					-- Email address (If NULL, do not send newsletter)
  points INT(8) NOT NULL,								-- Points
  PRIMARY KEY (customerID),								
  UNIQUE INDEX customerID_UNIQUE (customerID ASC)
)
ENGINE = InnoDB;


-- ENTITY staff
-- Describes all current employees of Juic'd
CREATE TABLE staff (
  staffID INT(12) NOT NULL AUTO_INCREMENT,			    -- unique PRIMARY KEY (assigned ID)
  snameFirst VARCHAR(45) NOT NULL,						-- First Name
  snameLast VARCHAR(45) NOT NULL,						-- Last Name
  staffRank VARCHAR(30) NOT NULL,						-- Rank, i.e. Manager, Trainee etc.
  reportsTo INT(12) NULL DEFAULT NULL,					-- Name of manager, (if rank is manager then leave as NULL)
  PRIMARY KEY (staffID),
  UNIQUE INDEX staffID_UNIQUE (staffID ASC),
    FOREIGN KEY (reportsTo)	REFERENCES staff (staffID)	-- derived from relation between managers and employees (reportsTo)
)
ENGINE = InnoDB;

	-- RELATION worksAt
	-- N-to-N relationship
	-- Links employees with the outlet/outlets they work at.
	CREATE TABLE worksAt (		
	  staffID INT(12) NOT NULL,						-- PRIMARY KEY (Staff ID <- staff)
	  outletID INT(8) NOT NULL,						-- PRIMARY KEY (Outlet ID <- outlets)
	  percentage DOUBLE NOT NULL,					-- percentage of time spent at outlet X
	  PRIMARY KEY (staffID, outletID),	 
		FOREIGN KEY (staffID) REFERENCES staff (staffID),
		FOREIGN KEY (outletID) REFERENCES outlets (outletID)
	)
	ENGINE = InnoDB;


-- ENTITY outlets
-- Describes all outlets of juic'd
CREATE TABLE outlets (
  outletID INT(8) NOT NULL AUTO_INCREMENT,		-- unique PRIMARY KEY (assigned ID)
  outletName VARCHAR(45) NOT NULL,				-- Name of shopping mall outlet is located within
  managerID INT(12) NOT NULL,					-- ID of manager currently running outlet (from relation managerOf)
  PRIMARY KEY (outletID),
  UNIQUE INDEX outletID_UNIQUE (outletID ASC),
    FOREIGN KEY (managerID) REFERENCES staff (staffID) -- derived from relation between managers and outlets (managerOf -> managerID)
)
ENGINE = InnoDB;

-- ENTITY orders
-- Describes all orders placed to date
CREATE TABLE orders (
  orderID INT(12) NOT NULL,						-- unique PRIMARY KEY (assigned ID)
  customerID INT(12) NOT NULL,					-- SECONDARY KEY (from relation orderPlaced)
  staffID INT(12) NOT NULL,						-- SECONDARY KEY (from relation orderPlaced)
  timedate DATETIME NOT NULL,					-- Time and Date of transaction format YYYY-MM-DD HH:MM:SS
  outletID INT(8) NOT NULL,					    -- SECONDARY KEY (from relation orderPlaced)
  day ENUM('Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday') NOT NULL,	-- Day
  PRIMARY KEY (orderID),												
  UNIQUE INDEX salesID_UNIQUE (orderID ASC),			
    FOREIGN KEY (customerID) REFERENCES customers (customerID),			-- derived from Relation between orders and customers (Customer -> orderPlaced)
    FOREIGN KEY (staffID , outletID) REFERENCES worksAt (staffID , outletID)			-- derived from Relation between orders and staff/outlet (worksAt -> orderPlaced)
)
ENGINE = InnoDB;

-- ENTITY items
-- Describes non-juice items sold at juic'd 
CREATE TABLE items (
  itemCode INT(12) NOT NULL,							-- unique PRIMARY KEY based on product code
  itemName VARCHAR(45) NOT NULL,						-- item name
  itemPrice DOUBLE NOT NULL,							-- price in AUD of item (as fixed price sold at all outlets)
  PRIMARY KEY (itemCode),
  UNIQUE INDEX itemID_UNIQUE (itemCode ASC)
)
ENGINE = InnoDB;

-- ENTITY juice
-- Describes juice items sold at juic'd
CREATE TABLE juice (
  juiceID INT(15) NOT NULL AUTO_INCREMENT,					-- PRIMARY KEY artificial ID
  juiceCupSize VARCHAR(8) NOT NULL,							-- Juice cup size
  sizeinML INT(4) NOT NULL,									-- size of cup in ML
  PRIMARY KEY (juiceID)
)
ENGINE = InnoDB;

-- ENTITY ingredientsJuice
-- Describes all possible ingredients for use in juices
CREATE TABLE ingredientsJuice (
  ingredientName VARCHAR(45) NOT NULL,				  -- unique PRIMARY KEY (Ingredient Name)
  unitPrice DOUBLE NOT NULL,						  -- unit price in AUD of ingredient (per ml)
  PRIMARY KEY (ingredientName),
  UNIQUE INDEX ingredientName_UNIQUE (ingredientName ASC)
)
ENGINE = InnoDB;

	-- RELATION hasNonJuice
	-- N-to-N relationship
	-- Links non-juice items with orders
	CREATE TABLE hasNonJuice (
	  orderID INT(12) NOT NULL,						-- PRIMARY FOREIGN KEY (from orders)
	  itemCode INT(12) NOT NULL,					-- PRIMARY FOREIGN KEY (from items)
	  itemQuantity INT(4) NOT NULL,					-- Quantity of item ordered
	  PRIMARY KEY (orderID, itemCode),
		FOREIGN KEY (orderID) REFERENCES orders (orderID),
		FOREIGN KEY (itemCode) REFERENCES items (itemCode)
	)
	ENGINE = InnoDB;
	
	-- RELATION hasJuice
	-- N-to-N relationship
	-- Links juices with orders

	CREATE TABLE hasJuice (
	  orderID INT(12) NOT NULL,						-- PRIMARY FOREIGN KEY (from orders)
	  juiceID INT(15) NOT NULL,						-- PRIMARY FOREIGN KEY (from juice)
	  juiceQuantity INT(4) NOT NULL,				-- Quantity of item ordered
	  PRIMARY KEY (juiceID, orderID),
	  UNIQUE INDEX orderID_UNIQUE (orderID ASC),
		FOREIGN KEY (juiceID) REFERENCES juice (juiceID),
		FOREIGN KEY (orderID) REFERENCES orders (orderID),
	)
	ENGINE = InnoDB;

	-- RELATION ConsistsOf
	-- N-to-N relationship
	-- Links ordered juices with the ingredients they are consisted of

	CREATE TABLE consistsOf (
	  ingredientName VARCHAR(45) NOT NULL,				-- PRIMARY KEY (from ingredientsJuice)
	  ingredientPercentage DOUBLE NOT NULL,				-- Percentage of ingredient in juice
	  juiceID INT(15) NOT NULL,							-- FOREIGN KEY (from juice)
	  PRIMARY KEY (ingredientName),
		FOREIGN KEY (ingredientName) REFERENCES ingredientsJuice (ingredientName),
		FOREIGN KEY (juiceID) REFERENCES juice (juiceID)
	)
	ENGINE = InnoDB;
