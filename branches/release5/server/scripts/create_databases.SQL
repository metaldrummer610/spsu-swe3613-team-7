DROP TABLE cm_gems;
DROP TABLE icd_9_descriptions;
DROP TABLE icd_10_cm_descriptions;

CREATE TABLE icd_9_descriptions (
	icd_9_code VARCHAR(10) PRIMARY KEY, 
	icd_9_description VARCHAR(500) DEFAULT 'No Description Found'
);

CREATE TABLE icd_10_cm_descriptions (
	icd_10_code VARCHAR(10) PRIMARY KEY,
	icd_10_description VARCHAR(500) DEFAULT 'No Description Found' 
);

CREATE TABLE cm_gems (
	icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
	flags CHAR(5),
	PRIMARY KEY (icd_9_code, icd_10_code, flags)
);

CREATE TABLE dx_codes (
	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
	dx_code VARCHAR(10) PRIMARY KEY
);


