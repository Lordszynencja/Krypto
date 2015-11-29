import java.math.BigInteger;
import java.util.Random;

public class RSA {
	BigInteger n;
	BigInteger e;
	BigInteger d[];
	BigInteger a[];
	BigInteger p[];
	BigInteger d0;
	int k;
	int l;
	int threads[];
	int threadResults[];
	BigInteger results[];
	RSAThread t[];
	
	public RSA() {
		Gen(1024,2);
	}
	
	public RSA(int keysize, int l) {
		Gen(keysize, l);
	}
	
	public void Gen(int keysize, int l) {
		threads=new int[l];
		threadResults=new int[l];
		for (int i=0;i<l;i++) {
			threads[i]=0;
			threadResults[i]=-1;
		}
		results=new BigInteger[l];
		t=new RSAThread[l];
		for (int i=0;i<l;i++) {
			t[i]=new RSAThread(i,this);
			t[i].start();
		}
		
		Random r=new Random();
		d=new BigInteger[l];
		a=new BigInteger[l-1];
		p=new BigInteger[l];
		BigInteger euler=BigInteger.ONE;
		n=BigInteger.ONE;
		k=keysize;
		this.l=l;
		e=BigInteger.ONE;
		
		for (int i=0;i<l;i++) {
			p[i]=BigInteger.probablePrime(k, r);
			n=n.multiply(p[i]);
			euler=euler.multiply(p[i].subtract(BigInteger.ONE));
		}
		do {
			e=e.add(BigInteger.ONE);
		} while(!(e.gcd(euler).equals(BigInteger.ONE)));
		d0=e.modInverse(euler);
		
		d[0]=d0.mod(p[0].subtract(BigInteger.ONE));
		for (int i=1;i<l;i++) {
			d[i]=d0.mod(p[i].subtract(BigInteger.ONE));
			a[i-1]=BigInteger.ONE;
			for (int j=1;j<i;j++) {
				a[i-1]=a[i-1].multiply(p[j].modInverse(p[i]));
			}
		}
	}
	
	public void DeGen() {
		for (int i=0;i<l;i++) {
			t[i].stop();
		}
	}
	
	public BigInteger Enc(BigInteger b) {
		return b.modPow(e, n);
	}
	
	public BigInteger Dec1(BigInteger b) {
		return b.modPow(d0, n);
	}
	
	public BigInteger Dec(BigInteger b) {
		BigInteger m[]=new BigInteger[l];
		BigInteger c[]=new BigInteger[l];
		
		for (int i=0;i<l;i++) {
			c[i]=b.mod(p[i]);
			m[i]=c[i].modPow(d[i],p[i]);
		}
		BigInteger u=m[0];
		BigInteger p1=p[0];
		BigInteger m1=m[0];
		for (int i=1;i<l;i++) {
			c[i]=b.mod(p1);
			m[i]=c[i].modPow(d[i],p[i]);
			u=m[i].subtract(m1).multiply(c[i]).mod(a[i-1]);
			p1=p1.multiply(p[i]);
			m1=m1.add(u.multiply(p1));
		}
		return m1;
	}
	
	public BigInteger[] ThreadEncode(BigInteger[] decoded) {
		int l1=decoded.length;
		BigInteger result[]=new BigInteger[l1];
		int set=0;
		boolean done[]=new boolean[l1];
		for (int i=0;i<l1;i++) {
			done[i]=false;
		}
		boolean isDone=false;
		//System.out.println("start kodowania");
		while (!isDone) {
			isDone=true;
			for (int i=0;i<l1;i++) {
				if (!done[i]) {
					isDone=false;
					i=l1;
				}
			}
			for (int i=0;i<l;i++) {
				if (threads[i]==0) {
					if (threadResults[i]>-1) {
						//System.out.println("watek "+i+" obliczyl "+threadResults[i]);
						result[threadResults[i]]=results[i];
						done[threadResults[i]]=true;
						threadResults[i]=-1;
					}
					if (set<l1) {
						//System.out.println("wyslano "+set+" do watku "+i);
						results[i]=decoded[set];
						threads[i]=1;
						threadResults[i]=set;
						set++;
					}
				}
			}
		}
		for (int i=0;i<l;i++) {
			if (threads[i]==0) {
				if (threadResults[i]>-1) {
					result[threadResults[i]]=results[i];
					threadResults[i]=-1;
				}
			}
		}
		return result;
	}
	
	public BigInteger[] ThreadDecode(BigInteger[] encoded) {
		int l1=encoded.length;
		BigInteger result[]=new BigInteger[l1];
		int set=0;
		boolean done[]=new boolean[l1];
		for (int i=0;i<l1;i++) {
			done[i]=false;
		}
		boolean isDone=false;
		//System.out.println("start kodowania");
		while (!isDone) {
			isDone=true;
			for (int i=0;i<l1;i++) {
				if (!done[i]) {
					isDone=false;
					i=l1;
				}
			}
			for (int i=0;i<l;i++) {
				if (threads[i]==0) {
					if (threadResults[i]>-1) {
						result[threadResults[i]]=results[i];
						done[threadResults[i]]=true;
						threadResults[i]=-1;
					}
					if (set<l1) {
						results[i]=encoded[set];
						threads[i]=2;
						threadResults[i]=set;
						set++;
					}
				}
			}
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				
			}
		}
		//System.out.println("koniec kodowania");
		for (int i=0;i<l;i++) {
			if (threads[i]==0) {
				if (threadResults[i]>-1) {
					result[threadResults[i]]=results[i];
					threadResults[i]=-1;
				}
			}
		}
		return result;
	}

	public static void main(String[] args) {
		RSA r=new RSA(2048,10);
		BigInteger wiadomosci[]=new BigInteger[12];
		wiadomosci[0]=new BigInteger("abcd".getBytes());
		wiadomosci[1]=new BigInteger("bcde".getBytes());
		wiadomosci[2]=new BigInteger("cdef".getBytes());
		wiadomosci[3]=new BigInteger("defg".getBytes());
		wiadomosci[4]=new BigInteger("abcd".getBytes());
		wiadomosci[5]=new BigInteger("bcde".getBytes());
		wiadomosci[6]=new BigInteger("cdef".getBytes());
		wiadomosci[7]=new BigInteger("defg".getBytes());
		wiadomosci[8]=new BigInteger("abcd".getBytes());
		wiadomosci[9]=new BigInteger("bcde".getBytes());
		wiadomosci[10]=new BigInteger("cdef".getBytes());
		wiadomosci[11]=new BigInteger("defg".getBytes());
		wiadomosci=r.ThreadEncode(wiadomosci);
		wiadomosci=r.ThreadDecode(wiadomosci);
		for (int i=0;i<12;i++) {
			System.out.println(new String(wiadomosci[i].toByteArray()));
		}
		r.DeGen();
	}

	class RSAThread extends Thread implements Runnable {
		int ID;
		RSA r;
		
		public RSAThread(int id,RSA r) {
			super();
			ID=id;
			this.r=r;
		}
		
		public void run() {
			//System.out.println(ID+"start");
			while(true) {
				if (threads[ID]==2) {
					//System.out.println("otrzymano "+threadResults[ID]+" do zdekodowania dla "+ID);
					results[ID]=r.Dec1(results[ID]);
					threads[ID]=0;
				} else if (threads[ID]==1) {
					//System.out.println("otrzymano "+threadResults[ID]+" do zakodowania dla "+ID);
					results[ID]=r.Enc(results[ID]);
					threads[ID]=0;
				} else {
					try {
						sleep(10);
					} catch (InterruptedException e) {
						System.out.println(ID);
					}
				}
			}
		}
	}
}
