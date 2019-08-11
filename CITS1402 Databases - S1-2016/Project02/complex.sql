-- more complex queries
-- Evan Huang 20916873

-- What SQL query will list the total number of customer orders per outlet (listed by outlet address) per day of the week? 
-- (In other words, Outlet 1s orders on Mondays, Tuesdays etc.)
SELECT O.address AS outlet, DATE_FORMAT(C.date, '%W') AS day, COUNT(*) AS totalOrders
FROM customerorder C
	JOIN outlet O
	ON C.outletID=O.jStoreId
GROUP BY C.outletID, day;

-- What SQL query will list the cup ids of all the JuiceCup that use more than three different ingredients? 
-- (You may assume that each row of comprises for a givenJuiceCup is actually a different ingredient.)
SELECT J.cupId
FROM JuiceCup J  
	JOIN comprises C
	ON J.cupId=C.cupId
GROUP BY J.cupId
HAVING COUNT(*)>3;

-- What single SQL query will list the names and addresses of all Juicd employees who are not working at any of the outlets at the moment?
SELECT E.name, E.address
FROM Employee E
	LEFT OUTER JOIN worksat W
	ON W.jEmpId=E.jEmpId
WHERE W.jEmpId is NULL;

-- What SQL query will list all the customer orders (by orderId) that consist only of juices (no non-juice items)?
SELECT C.orderID
FROM customerorder C
	LEFT OUTER JOIN hasNonJuice N
	ON C.orderID=N.orderID
WHERE N.orderID is NULL
ORDER BY C.orderID ASC;

-- FUNCTIONS
DELIMITER ++
--

-- Write a function juiceCupCost(id INT) RETURNS DOUBLE that will be called with the id of a JuiceCup and then return the cost (in cents) of that particular JuiceCup. 
DROP FUNCTION IF EXISTS juiceCupCost;
CREATE FUNCTION juiceCupCost(id INT) RETURNS DOUBLE
BEGIN
	DECLARE JuiceCost DOUBLE;
	
	SELECT SUM(J.perMl* percentage/100 * JC.size) INTO JuiceCost
	FROM comprises C 
		JOIN juice J 
		ON J.jId=C.juiceId 
		JOIN juiceCup JC
		ON C.cupId=JC.cupId
	WHERE C.cupid = id;
	
	RETURN (JuiceCost);
END++

-- Write a function juiceOrderCost(id INT) RETURNS DOUBLE that returns the total price of the juice-component of an order.
DROP FUNCTION IF EXISTS juiceOrderCost;
CREATE FUNCTION juiceOrderCost(id INT) RETURNS DOUBLE
BEGIN
	DECLARE JuiceCost DOUBLE DEFAULT 0;
	DECLARE numRows INT DEFAULT 0;
	DECLARE numDone INT DEFAULT 0;
	DECLARE JID INT(11);
	DECLARE Jquantity INT(11);
	DECLARE totalcost DOUBLE DEFAULT 0;
	
	DECLARE cupCursor CURSOR FOR
		SELECT cupID, quantity
		FROM hasJuice
		WHERE orderID = id;

	
	OPEN cupCursor;
		SELECT FOUND_ROWS() INTO numRows;
		WHILE numDone < numRows DO
			FETCH cupCursor INTO JID, Jquantity;
			SELECT juiceCupCost(JID) INTO JuiceCost;
			SET totalCost = totalCost + JuiceCost * Jquantity;
			SET numDone = numDone + 1;
		END WHILE;
		RETURN totalCost;
	CLOSE cupCursor;
END++

-- REVERT DELIMITER
DELIMITER ;