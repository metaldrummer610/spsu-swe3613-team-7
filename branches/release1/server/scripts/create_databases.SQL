DROP TABLE cm_gems;
DROP TABLE pcs_gems;
DROP TABLE icd_9_descriptions;
DROP TABLE icd_10_cm_descriptions;
DROP TABLE icd_10_pcs_descriptions;

CREATE TABLE icd_9_descriptions (
	code VARCHAR(10) PRIMARY KEY, 
	description VARCHAR(500) DEFAULT 'No Description Found'
);

CREATE TABLE icd_10_cm_descriptions (
	code VARCHAR(10) PRIMARY KEY,
	description VARCHAR(500) DEFAULT 'No Description Found' 
);

CREATE TABLE icd_10_pcs_descriptions (
	code VARCHAR(10) PRIMARY KEY,
	description VARCHAR(500) DEFAULT 'No Description Found'
);

CREATE TABLE cm_gems (
	icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
	icd_10_code VARCHAR(10) REFERENCES icd_10_cm_descriptions,
	flags INTEGER,
	PRIMARY KEY (icd_9_code, icd_10_code, flags)
);

CREATE TABLE pcs_gems (
	icd_9_code VARCHAR(10) REFERENCES icd_9_descriptions,
	icd_10_code VARCHAR(10) REFERENCES icd_10_pcs_descriptions,
	flags INTEGER,
	PRIMARY KEY (icd_9_code, icd_10_code, flags)
);
