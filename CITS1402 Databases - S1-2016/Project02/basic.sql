-- basic SQL queries
-- Evan Huang 20916873

--  What SQL query will produce the row of customer information for the customer with Juicd card number 1000?
SELECT * 
FROM Customer 
WHERE jCardNum=1000;

-- What SQL query will determine the total number of orders in the database?
SELECT COUNT(*) AS totalOrders 
FROM customerorder;

--  What SQL query will list the names and addresses of the outlet managers?
SELECT E.name, E.address 
FROM manages M 
	JOIN Employee E 
	ON M.jEmpId=E.jEmpId;

-- What SQL query will list the names and addresses of the employees who work fulltime at a single Juicd store?
SELECT E.name, E.address, E.jEmpId
FROM Employee E 
	JOIN worksat W 
	ON E.jEmpId=W.jEmpId
WHERE W.percentage=100;

-- What SQL query will list the names, addresses and total working percentage of all Juicd employees (so someone who works 50% at one store and 25% at another store works a total of 75% of their time). Only include employees that are currently working.
SELECT E.name, E.address, SUM(W.percentage) AS totalPercentage 
FROM Employee E 
	JOIN worksat W 
	ON E.jEmpId=W.jEmpId 
GROUP BY E.jEmpId;

-- What query will produce a table listing the minimum, maximum and average number of Juicd points outstanding?
SELECT MIN(jPoints) AS minimum, MAX(jPoints) AS maximum, AVG(jPoints) AS average 
FROM customer;

-- What SQL query will list each line manager (by name) together with the number of employees they supervise? (supervision is in the lineMgr table)?
SELECT E.name AS lineManager, COUNT(*) AS supervises 
FROM Employee E 
	JOIN linemgr L 
	ON E.jEmpId=L.Supervisor 
GROUP BY E.jEmpId;

-- What SQL query will list the address of each outlet, together with the total number of orders that they have served?
SELECT O.address AS outlet, COUNT(*) AS totalOrders 
FROM outlet O 
	JOIN customerorder CO 
	ON O.jStoreId=CO.outletID 
GROUP BY O.jStoreId;

-- What SQL query will list the actual juices by name and their percentages for this particular juice cup (i.e. the juice with cupId 1000)?
SELECT J.jName AS juiceName, C.percentage 
FROM comprises C 
	JOIN juice J 
	ON J.jId=C.juiceId 
WHERE C.cupid = 1000;

-- What SQL command will determine the price in cents of the JuiceCup with cupId 1000? (The table Juice contains the price in cents-per-ml of each juice).
SELECT SUM(J.perMl * percentage/100 * JC.size) AS JuiceCost
FROM comprises C 
	JOIN juice J 
	ON J.jId=C.juiceId 
	JOIN juiceCup JC
	ON C.cupId=JC.cupId
WHERE C.cupid = 1000;