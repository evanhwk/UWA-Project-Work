-- additional SQL queries
-- Evan Huang 20916873

-- VIEWS
-- Create a view CustomerPricedOrder with columns date DATE, customerId INT, orderId INT and orderCost DOUBLE that provides a more accessible way for the DB user to run queries regarding customer orders.
DROP VIEW IF EXISTS CustomerPricedOrder;
CREATE VIEW CustomerPricedOrder AS
	SELECT date, customerID, orderID, totalOrderCost(orderID) AS orderCost
	FROM customerorder;

-- Shows total amount of money a customer spends on a monthly basis
DROP VIEW IF EXISTS MonthlyCustomerSpending;
CREATE VIEW MonthlyCustomerSpending AS
	SELECT DATE_FORMAT(CPO.date, '%Y') AS year, DATE_FORMAT(CPO.date, '%M') AS month, CPO.customerID, SUM(CPO.orderCost) AS totalSpending
		FROM CustomerPricedOrder CPO
		GROUP BY customerID, year, month;

-- FUNCTIONS
DELIMITER ++
-- 

-- Returns nonjuice cost for given product ID (in cents)
DROP FUNCTION IF EXISTS nonJuiceCost;
CREATE FUNCTION nonJuiceCost(id INT) RETURNS DOUBLE
BEGIN
	DECLARE NonJuiceCost DOUBLE;
	
	SELECT perItem*100 INTO NonJuiceCost
	FROM nonJuice
	WHERE prodID = id;
	
	RETURN NonJuiceCost;
END++

-- Returns nonjuice cost for given order ID
DROP FUNCTION IF EXISTS nonJuiceOrderCost;
CREATE FUNCTION nonJuiceOrderCost(id INT) RETURNS DOUBLE
BEGIN
	DECLARE NJuiceCost DOUBLE DEFAULT 0;
	DECLARE numRows INT DEFAULT 0;
	DECLARE numDone INT DEFAULT 0;
	DECLARE NJID INT(11);
	DECLARE NJquantity INT(11);
	DECLARE totalcost DOUBLE DEFAULT 0;
	
	DECLARE NJCursor CURSOR FOR
		SELECT prodID, quantity
		FROM hasNonJuice
		WHERE orderID = id;
		
	OPEN NJCursor;
		SELECT FOUND_ROWS() INTO numRows;
		WHILE numDone < numRows DO
			FETCH NJCursor INTO NJID, NJquantity;
			SELECT NonjuiceCost(NJID) INTO NJuiceCost;
			SET totalCost = totalCost + NJuiceCost * NJquantity;
			SET numDone = numDone + 1;
		END WHILE;
		RETURN totalCost;
	CLOSE NJCursor;
END++

-- Write a stored function totalOrderCost(id INT) RETURNS DOUBLE that returns the total cost (in cents) of the order with order number id.
DROP FUNCTION IF EXISTS totalOrderCost;
CREATE FUNCTION totalOrderCost(id INT) RETURNS DOUBLE
BEGIN
	DECLARE JuiceCost DOUBLE DEFAULT 0;
	DECLARE NonJuiceCost DOUBLE DEFAULT 0;
	DECLARE totalCost DOUBLE DEFAULT 0;
	
	SELECT juiceOrderCost(id) INTO JuiceCost;
	SELECT nonJuiceOrderCost(id) INTO NonJuiceCost;
	SET totalCost = JuiceCost + NonJuiceCost;
	
	RETURN totalCost;
END++

-- PROCEDURES
--

-- Juicd runs a Customer Of The Month promotion, and sends coupons and other rewards to the highest spending customer each month. 
-- Write a stored procedure listCofM()that creates a table customerOfMonth(year INT, month TEXT, COfM INT, cOfMemail TEXT) that lists for each year and month (given as "January", "February" etc) the customer number and email of the biggest spender for that month

DROP PROCEDURE IF EXISTS listCofM;
CREATE PROCEDURE listCofM()
BEGIN
	DROP TABLE IF EXISTS customerOfMonth;
	CREATE TABLE customerOfMonth AS 
		SELECT MCS.year, MCS.month, MCS.customerID AS CofM, C.email AS cofMemail
			FROM Customer C
				JOIN MonthlyCustomerSpending MCS
				ON C.jCardNum = MCS.customerID
			GROUP BY MCS.year, MCS.month;
			
	ALTER TABLE customerOfMonth MODIFY year INT;
	ALTER TABLE customerOfMonth MODIFY month TEXT;
	ALTER TABLE customerofMonth MODIFY cofMemail TEXT;
END++

-- REVERT DELIMITER
DELIMITER ;